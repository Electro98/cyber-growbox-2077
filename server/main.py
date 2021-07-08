import sys
import requests
import pandas
import json

from flask import Flask, request, render_template
from flask.json import jsonify
from flask_socketio import SocketIO, send, emit
from influxdb import InfluxDBClient
from plotly.io import to_json
from plotly import graph_objs
from functools import wraps
from datetime import datetime

app = Flask(__name__)
socketio = SocketIO(app)

light_state = [{'name': lamp, 'val': 0} for lamp in ('Вкл.\nПульт', 'Лампы', 'Насос')]
motors_state = 0


def work_with_base(function):
    """Декоратор для упрощения работы с базой."""
    @wraps(function)
    def modificied_function(*args, **kwargs):
        client = InfluxDBClient(host='localhost', port=8086)
        client.switch_database('test')
        result = function(*args, client=client, **kwargs)
        client.close()
        return result
    return modificied_function


# Костыль для того, чтобы никто не ругался,
#    что javascript нельзя ответы на запросы получать
@app.after_request
def add_cors_headers(response):
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response


@app.route('/data', methods=['POST'])
@work_with_base
def post(self, client=None):
    if not client:
        print('Error with connection to DB!')
        return 'Error with DB.'
    data = request.json
    print(data, file=sys.stderr)
    now = int(datetime.today().timestamp() * 10**9)
    for key, value in data.items():
        client.write(f'{key} value={value} {now}')
    # emit('update', (datetime.today().strftime('%Y-%m-%d %H:%M:%S'), 2), namespace='/real-time-graph', broadcast=True)
    return 'Ok.'


@app.route('/command', methods=['POST'])
def add_commands():
    data = request.json
    print(data)
    if 'command' not in data.keys() and 'arg' not in data.keys():
        return 'Failed.'
    if data['command'] in ('A', 'D'):
        light_state[int(data['arg'])]['val'] = int(data['command'] == 'A')
    elif data['command'] in ('R', 'L'):
        global motors_state  # Осуждаю, честно, дайте идей, как это поправить
        motors_state += int(data['arg'])
    return 'Success!'


@app.route('/get_command')
def get_command():
    commands = {'light': [bulb['val'] for _, bulb in enumerate(light_state[1:])]}
    commands['control'] = light_state[0]['val']
    global motors_state
    if motors_state:
        commands['motors'] = motors_state
    motors_state = 0
    return jsonify(commands)


@app.route('/bulbs')
def bulbs_update():
    return jsonify({f'bulb{num}': bulb['val'] for num, bulb in enumerate(light_state)})


@app.route('/view')
@work_with_base
def show_last_state(client=None):
    if not client:
        print('Error with connection to DB!')
        return 'Error with DB.'

    meas_names = list(point['name'] for point in client.query('SHOW measurements').get_points())
    data = client.query(f'SELECT last(value) from {", ".join(meas_names)}')
    data = {meas_names[_id]: point["last"] for _id, point in enumerate(data.get_points())}
    return render_template('data.html', data=data)


@app.route('/graph')
@work_with_base
def graph(client=None):
    if not client:
        print('Error with connection to DB!')
        return 'Error with DB.'

    meas_names = list(point['name'] for point in client.query('SHOW measurements').get_points())
    measurement = meas_names[0]
    if request.args.get('plot') and request.args['plot'] in meas_names:
        measurement = request.args['plot']
    data = client.query(f"SELECT value FROM {measurement}")
    df = pandas.DataFrame(list(data.get_points()))
    df['time'] = pandas.to_datetime(df['time'])
    print(df)
    graph = graph_objs.Figure()
    graph.add_trace(graph_objs.Scatter(x=df['time'], y=df['value']))
    graph.update_layout(title=f"График {measurement}",
                        xaxis_title="Дата",
                        yaxis_title=measurement,)
    return render_template('graph.html', plot=to_json(graph))


@socketio.on('connect', namespace='/real-time-graph')
def handle_message():
    print('client connected')
    emit('my response', {'data': 'Connected'})


@socketio.on('disconnect', namespace='/real-time-graph')
def handle_json():
    print('client disconnected')


@app.route('/')
@app.route('/index')
def index():
    return render_template('index.html', data=enumerate(light_state))


if __name__ == '__main__':
    socketio.run(app, debug=True)
    # socketio.run(app, host='0.0.0.0')
