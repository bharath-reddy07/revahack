from flask import Flask, render_template

from time import time

app = Flask(__name__)

MASTER_PASSWORD = "lmao"

@app.route('/')
def home_page():
    return render_template('home_page.html')

@app.route('/get_otp')
def get_otp():
    hex_master = [ord(l) for l in MASTER_PASSWORD]
    num1 = 0

    for i in range(len(hex_master)):
        num1 += hex_master[i]<<8*(3-i)

    india_time = time() + 5.555555*60*60 - 385
    print("TIME: ", india_time)
    cur_secs = int(india_time/30)
    cur_secs = str(cur_secs)
    pairs = [int(cur_secs[i])*10 + int(cur_secs[i+1]) for i in range(0,8,2)]
    
    num2 = 0
    for i in range(len(pairs)):
        num2 += pairs[i] << 8*(3-i)

    totp = hex(num1 + num2)
    print(num1, num2)
    print(totp)



    return {"otp": totp}

@app.route('/backup')
def backup():
    return render_template('backup.html')

@app.route("/getBackup")
def getBackup():
    with open("backup.data", 'rb') as f:
        data = f.read()
    return {'data': str(data)}





#Checks if the run.py file has executed directly and not imported
if __name__ == '__main__':
    app.run(debug=True,  port=8000, host="0.0.0.0")