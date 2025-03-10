"""
Top level module for SPL display kiosk application
"""
import argparse
import os
import socket

from threading import Lock
from flask import Flask, render_template, request, send_from_directory
from flask_socketio import SocketIO, emit # pylint grumbles about missing package here

app = Flask(__name__)
socketio = SocketIO(app)
GTHREAD = None
thread_lock = Lock()
PORTNO = None

class SqlSocket:
    """
    Wrapper class to initialize data for the SPL monitor's background thread
    """
    def __init__(self, hostip, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((hostip, port))
        self.socket.settimeout(0.5)

    def __del__(self):
        pass

    def __repr__(self):
        return f"SqlSocket(socket='{self.socket}')"

    def backgroundThread(self):
        """
        Read from the SPL server and push updates on data available
        """
        while True:
            try:
                spl = self.socket.recv(1024).decode()
            except TimeoutError:
                pass
            else:
                socketio.emit('spl update', {'data' : f"{float(spl):.1f}"})
                socketio.sleep(0)

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
    if PORTNO is None:
        print("Connect to port 1234")
        sqlSocket = SqlSocket('127.0.0.1', 1234)
    else:
        print(f"Connect to port {PORTNO}")
        sqlSocket = SqlSocket('127.0.0.1', int(PORTNO))

    global GTHREAD
    with thread_lock:
        if GTHREAD is None:
            GTHREAD = socketio.start_background_task(sqlSocket.backgroundThread)
    emit('my_response', {'data': 'Connected', 'count': 0})


@socketio.on('disconnect')
def testDisconnect(reason):
    """ Disconnect from client """
    print('Client disconnected', request.sid, reason)

def catchIt():
    """ Catch the KeyboardInterrupt """
    print('Interrupted')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="SPL kiosk client/server")

    parser.add_argument("portno", help="The port number of the SPL monitor process")

    args = parser.parse_args()

    PORTNO = args.portno
    socketio.run(app)
