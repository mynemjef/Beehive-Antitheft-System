from flask import Flask, request
import subprocess

app = Flask(__name__)

@app.route('/update', methods=['POST'])
def update():
    data = request.form.to_dict()
    pir_states = [int(data.get('pir1')), int(data.get('pir2')), int(data.get('pir3')), int(data.get('pir4'))]
    button_state = int(data.get('button'))

    if button_state == 0:
        print("ALERT: Beehive moved!")
        subprocess.run(["echo", "ALERT: Beehive moved!"])
    if any(pir_states):
      print("ALERT: Motion detected!")
        subprocess.run(["echo", "ALERT: Motion detected!"])

    return 'OK'

if __name__ == '__main__':
    app.run(debug=False, host='0.0.0.0', port=8000)
