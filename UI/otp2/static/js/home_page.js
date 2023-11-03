document.getElementById("process-btn").addEventListener("click", ()=>{
    request_otp()
    

})

async function request_otp(){
    let response = await fetch("/get_otp").then((response)=>response.json())
    console.log(response['otp'])

    const para = document.createElement("p")
    para.id = "result"
    para.innerHTML = response['otp']

    // document.getElementById("result-container").innerHTML = `<p id="result" >${response['otp']}</p>`
    document.getElementById("result-container").appendChild(para)


    setTimeout(() => {
        para.style.opacity = '0';
    }, 20000);
    errorBox.addEventListener('transitionend', () => errorBox.remove());


}

