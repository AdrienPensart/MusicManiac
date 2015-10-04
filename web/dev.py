import os
from flask_failsafe import failsafe

@failsafe
def create_app():
    from musicmaniac import app
    return app

listening_host = '0.0.0.0'
listening_port = os.environ.get("PORT", "3001")
listening_address = listening_host + ':' + listening_port

if __name__ == "__main__":
    create_app().run(host=listening_host, port=int(listening_port), debug = True)
