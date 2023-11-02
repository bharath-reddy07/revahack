from otp import app
from flask import Flask, render_template, request, redirect, url_for

@app.route('/')
def home_page():
    return render_template('home_page.html')

@app.route('/process_otp', methods=['POST'])
def process_otp():
    otp = request.form.get('otp')
    # Add your OTP processing logic 
    processed_otp = otp[::-1] 
    return redirect(url_for('result_page', result=processed_otp))

@app.route('/result_page')
def result_page():
    result = request.args.get('result', default='')
    return render_template('result_page.html', result=result)



