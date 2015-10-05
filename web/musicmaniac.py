from flask import Flask, request, Response, jsonify
from flask.ext.sqlalchemy import SQLAlchemy
from flask_httpauth import HTTPBasicAuth
from flask_limiter import Limiter
from flask_restful import Resource, Api
import json

app = Flask(__name__)
app.config['PROPAGATE_EXCEPTIONS'] = True
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:////tmp/musicmaniac.db'
db = SQLAlchemy(app)
api = Api(app)
auth = HTTPBasicAuth()
limiter = Limiter(app, global_limits=["2000 per day", "500 per hour"])

users = {
    "daemon": "123daemon321",
    "crunch": "123crunch321"
}

@auth.get_password
def get_pw(username):
    if username in users:
        return users.get(username)
    return None

@app.route("/music", methods=['POST'])
@auth.login_required 
def music():
    print('Getting a new music update from ', auth.username())
    music = request.get_json(force=True)
    print music
    return (jsonify(music), 200)
    
if __name__ == "__main__":
    app.run()

