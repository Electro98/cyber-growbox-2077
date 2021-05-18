import sys
import sqlite3
import requests
import pandas
import json

from flask import Flask, request, render_template
from flask.json import jsonify
from flask.views import MethodView
from plotly.utils import PlotlyJSONEncoder
from plotly import graph_objs
from functools import wraps

app = Flask(__name__)

light_state = [{'name': lamp, 'val': 0} for lamp in ('Управление', 'Лампы', 'Насос')]
motors_state = 0


def work_with_base(function):
    """Декоратор для упрощения работы с базой."""
    @wraps(function)
    def modificied_function(*args, **kwargs):
        conn = sqlite3.connect("sensors_data.db")
        curs = conn.cursor()
        result = function(*args, curs=curs, **kwargs)
        conn.commit()
        conn.close()
        return result
    return modificied_function


# Костыль для того, чтобы никто не ругался,
#    что javascript нельзя ответы на запросы получать
@app.after_request
def add_cors_headers(response):
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response


class SensorsAPI(MethodView):
    # Про MethodView:
    # https://flask.palletsprojects.com/en/1.1.x/api/#flask.views.MethodView
    # Быстрый старт:
    # https://flask.palletsprojects.com/en/1.1.x/quickstart/
    def get(self):
        # тестовая вьюха для имитации отправки данных с датчиков
        state = {
            'date': '2021-04-07',
            'time': '21:23',
            'light_1': 238,
            'co2': 404,
        }
        return jsonify(state)

    @work_with_base
    def post(self, *, curs):
        # Столбцы таблицы
        # date time light_1 temp_water tds co2
        print(request)
        data = request.json
        print(data, file=sys.stderr)
        # По идеи тут должен быть вызов функции,
        #   которая асинхронно записывает данные в БД
        #
        # Но пока идёт тест...
        # Ахтунг
        curs.execute(f"INSERT INTO sensors (date, time) VALUES (\'{data['date']}\', \'{data['time']}\')")
        # return 'OK'
        return render_template('data.html', data=data)


app.add_url_rule('/data', view_func=SensorsAPI.as_view('counter'))


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
def show_last_state(*, curs):
    # тестовая вьюха для обзора последних данных с датчиков
    curs.execute("SELECT * FROM sensors WHERE ROWID = (SELECT MAX(ROWID) FROM sensors)")
    data = curs.fetchall()[0]
    # Столбцы таблицы
    # date time light_1 temp_water tds co2
    headers = ('Дата: ', 'Время: ',
               'Данные о свете: ',
               'Температура раствора',
               'Концентрация солей:',
               'Концентрация углекислоты',)

    data = {headers[_id]: elem for _id, elem in enumerate(data)}
    return render_template('data.html', data=data)


@app.route('/graph')
@work_with_base
def graph_example(*, curs):
    data = curs.execute("SELECT * FROM sensors").fetchall()
    df = pandas.DataFrame(data, columns=['date', 'time', 'light_1', 'temp_water', 'tds', 'co2'])
    for column in ('time', 'light_1', 'temp_water', 'tds'):
        del df[column]
    graph = [
        graph_objs.Scatter(
            x=df['date'],
            y=df['co2'],
        )
    ]
    graphJSON = json.dumps(graph, cls=PlotlyJSONEncoder)
    return render_template('graph.html', plot=graphJSON)


@app.route('/')
@app.route('/index')
def index():
    return render_template('index.html', data=enumerate(light_state))


if __name__ == '__main__':
    # или через консоль:
    # python -m flask run
    app.run(debug=True)
    # app.run(host='0.0.0.0')
