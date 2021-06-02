import sys
import sqlite3
import requests
import pandas
import json

from flask import Flask, request, render_template
from flask.json import jsonify
from flask.views import MethodView
from plotly.io import to_json
from plotly import graph_objs
from functools import wraps
from datetime import datetime

app = Flask(__name__)

light_state = [{'name': lamp, 'val': 0} for lamp in ('Вкл.\nПульт', 'Лампы', 'Насос')]
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
        data = request.json
        print(data, file=sys.stderr)
        # По идеи тут должен быть вызов функции,
        #   которая асинхронно записывает данные в БД
        #
        # Но пока идёт продуктион...
        # Ахтунг
        now = datetime.today()
        collumns_info = curs.execute("PRAGMA table_info(\'sensors\')").fetchall()
        collums_name = {collumn_info[1] for collumn_info in collumns_info}
        corrects_info_name = collums_name.intersection(set(data.keys()))
        data = {key: val for key, val in data.items() if key in corrects_info_name}
        data['date'] = now.strftime("'%Y-%m-%d'")
        data['time'] = now.strftime("'%H:%M:%S'")
        db_request = f"INSERT INTO sensors ({', '.join(data.keys())}) VALUES ({', '.join(map(str, data.values()))})"
        curs.execute(db_request)
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
def graph(*, curs):
    collumns_info = curs.execute("PRAGMA table_info(\'sensors\')").fetchall()
    collums_name = [collumn_info[1] for collumn_info in collumns_info[2:]]
    data_collumn = 'co2'
    if request.args.get('plot') and request.args['plot'] in collums_name:
        data_collumn = request.args['plot']
    data = curs.execute(f"SELECT date, time, {data_collumn} FROM sensors").fetchall()
    df = pandas.DataFrame(data, columns=['date', 'time', data_collumn])
    df['date'] = pandas.to_datetime(df['date'] + ' ' + df['time'])
    graph = graph_objs.Figure()
    graph.add_trace(graph_objs.Scatter(x=df['date'], y=df[data_collumn]))
    graph.update_layout(title=f"График {data_collumn}",
                        xaxis_title="Дата",
                        yaxis_title=data_collumn,)
    return render_template('graph.html', plot=to_json(graph))


@app.route('/')
@app.route('/index')
def index():
    return render_template('index.html', data=enumerate(light_state))


if __name__ == '__main__':
    # или через консоль:
    # python -m flask run
    app.run(debug=True)
    # app.run(host='0.0.0.0')
