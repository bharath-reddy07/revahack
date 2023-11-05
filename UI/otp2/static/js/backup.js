document.getElementById("backup-button").addEventListener("click", async ()=>{
    let data = await getBackup()
    console.log(data)

    const para = document.createElement("p")
    para.id = "dataResult"
    para.innerHTML = data

    document.getElementById("backup-content").append(para)

    

})

async function getBackup(){
    let response = await fetch("/getBackup").then((response)=>response.json())
    return response['data']

}