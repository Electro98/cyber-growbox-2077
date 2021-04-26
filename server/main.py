import sys
import sqlite3
import requests
from flask import Flask, request, render_template
from flask.json import jsonify
from flask.views import MethodView


app = Flask(__name__)

light_state = [0, 0]
commands_stack = []


def work_with_base(function):
    """Декоратор для упрощения работы с базой."""
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


@app.route('/command')
def add_command():
    args = request.args.to_dict()
    print(args)
    if 'command' not in args.keys() and 'arg' not in args.keys():
        return 'Failed.'
    if request.args['command'] in ('A', 'D'):
        light_state[int(args['arg'])] = int(args['command'] == 'A')
    else:
        commands_stack.append(args['command'] + args['arg'])
    return 'Success!'


@app.route('/get_command')
def get_command():
    commands = [f'A{num+1}' if bulb else f'D{num+1}' for num, bulb in enumerate(light_state)]
    commands.extend(commands_stack)
    if request.args.to_dict().get('clear'):
        commands_stack.clear()
    return ''.join(commands)


@app.route('/bulbs')
def bulbs():
    return str(light_state[0]) + str(light_state[1])


@app.route('/view')
@work_with_base
def show_state(*, curs):
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


@app.route('/')
def hello_world():
    return render_template('UI.html', data=light_state)


if __name__ == '__main__':
    # или через консоль:
    # python -m flask run
    # app.run(debug=True)
    app.run(host='0.0.0.0')
