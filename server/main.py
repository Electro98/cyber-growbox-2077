import sys
import sqlite3
import requests
from flask import Flask, request, render_template
from flask.json import jsonify
from flask.views import MethodView


app = Flask(__name__)


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

    def post(self):
        # Столбцы таблицы
        # date time light_1 temp_water tds co2
        data = request.form
        print(data, file=sys.stderr)
        # По идеи тут должен быть вызов функции, 
        #   которая асинхронно записывает данные в БД
        #
        # Но пока идёт тест...
        # Ахтунг
        conn = sqlite3.connect("sensors_data.db")
        curs = conn.cursor()
        curs.execute(f"INSERT INTO sensors (date, time) VALUES (\'{data['date']}\', \'{data['time']}\')")
        conn.commit()
        conn.close()
        # return 'OK'
        return render_template('data.html', data=data)


app.add_url_rule('/data', view_func=SensorsAPI.as_view('counter'))


@app.route('/send')
def send_state():
    # тестовая вьюха для имитации отправки данных с датчиков 
    state = {
        'date': '2021-04-07',
        'time': '21-23',
        'light_1': 238,
        'co2': 404,
    }
    # ручками отправляем POST запрос на нужны url
    res = requests.post('http://localhost:5000/data', data=state)
    # сами возвращаем html где уже отрендерен шаблон в качестве ответа 
    return (res.text, res.status_code, res.headers.items())


@app.route('/')
def hello_world():
    # Почему бы и нет
    return 'Hello, World!'


if __name__ == '__main__':
    # или через консоль:
    # python -m flask run
    app.run(debug = True)
