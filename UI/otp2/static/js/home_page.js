document.getElementById("process-btn").addEventListener("click", ()=>{
    request_otp()
    

})

async function request_otp(){
    let response = await fetch("/get_otp").then((response)=>response.json())
    console.log(response['otp'])
    document.getElementById("result-container").innerHTML = `<p id="result" style="background-color: #161a30; padding: 10px; border: 1px solid #bce8f1; border-radius: 5px;font-size:xx-large; border-color: #c5c7d2;">${response['otp']}</p>`

}

