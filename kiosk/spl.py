"""
Top level module for SPL display kiosk application
"""
import os
import socket

from threading import Lock
from flask import Flask, render_template, request, send_from_directory
from flask_socketio import SocketIO, emit

GTHREAD = None
thread_lock = Lock()

class SqlSocket:
    """
    Wrapper class to initialize data for the SPL monitor's background thread
    """
    def __init__(self, hostip, port, sio):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((hostip, port))
        self.socket.settimeout(0.5)
        self.sio = sio

    def __del__(self):
        pass

    def __repr__(self):
        return f"SqlSocket(socket='{self.socket}')"

    def background_thread(self):
        """
        Read from the SPL server and push updates on data available
        """
        while True:
            try:
                msg = self.socket.recv(1024).decode()
            except TimeoutError:
                pass
            else:
                spl = msg.split(";")[-2]
                print(f"{float(spl):.1f}")
                self.sio.emit('spl update', {'data' : f"{float(spl):.1f}"})
                self.sio.sleep(0)

def create_app(portno):
    """
    Flask app creation stuff. The input argument is the TCP port number
    """
    app = Flask(__name__)
    socketio = SocketIO(app)

    @app.route("/")
    def index():
        """ render the page """    
        return render_template('index.html',
                               async_mode=socketio.async_mode,
                               src='/home/pete/working/SPL/kiosk/templates/segment-display.js')

    @app.route('/js/<path:filename>')
    def serve_static(filename):
        root_dir = os.path.dirname(os.getcwd())
        return send_from_directory(os.path.join(root_dir, 'static', 'js'), filename)

    @socketio.event
    def connect():
        """
        Connect to the SPL monitor program's TCP server and spin up the reader thread
        """
        if portno is None:
            print("Connect to default port 1234", flush=True)
            sql_socket = SqlSocket('127.0.0.1', 1234, socketio)
        else:
            print(f"Connect to port {portno}", flush=True)
            sql_socket = SqlSocket('127.0.0.1', int(portno), socketio)

            global GTHREAD
            with thread_lock:
                if GTHREAD is None:
                    GTHREAD = socketio.start_background_task(sql_socket.background_thread)
                    emit('my_response', {'data': 'Connected', 'count': 0})


    @socketio.on('disconnect')
    def test_disconnect(reason):
        """ Disconnect from client """
        print('Client disconnected', request.sid, reason, flush=True)

    return app
