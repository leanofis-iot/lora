(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let loraForm = document.querySelector('#lora-form');
    let mainForm = document.querySelector('#main-form'); 
    let loraTemp = document.querySelectorAll('template')[0];   
    let generalTemp = document.querySelectorAll('template')[1];
    let analogTemp = document.querySelectorAll('template')[2];
    let digitalTemp = document.querySelectorAll('template')[3];
    let modbusTemp = document.querySelectorAll('template')[4];
    let timeTemp = document.querySelectorAll('template')[5];
    let lorasDiv = document.querySelector("#loras");
    let generalsDiv = document.querySelector("#generals");
    let analogsDiv = document.querySelector("#analogs");
    let digitalsDiv = document.querySelector("#digitals");
    let modbusesDiv = document.querySelector("#modbuses");
    let timesDiv = document.querySelector("#times"); 
    let loraGetBut = document.querySelector("#lora-get");
    let loraSaveBut = document.querySelector("#lora-save");          
    let connectBut = document.querySelector("#connect");    
    let timeBut = document.querySelector('#set-time');
    let readBut = document.querySelector('#read');
    let getBut = document.querySelector('#get');
    let saveBut = document.querySelector('#save');     
    let port;
    let statusDisp = document.querySelector('#status');

    let numAn = 2, numDg = 2, numMo = 8, numTm = 2;
    let items;

    //at+set_config=lora:app_eui:70B3D57ED001EF7F
        
    function create() {
      statusDisp.textContent = "";
      let clon;
      let buts;
      let divs;
      // LoRaWAN
      clon = loraTemp.content.cloneNode(true);     
      lorasDiv.appendChild(clon);              
      // Generals
      clon = generalTemp.content.cloneNode(true);     
      generalsDiv.appendChild(clon);       
      // Analogs
      for (let i = 0; i < numAn; i++) {
        clon = analogTemp.content.cloneNode(true);
        analogsDiv.appendChild(clon); 
      }      
      buts = analogsDiv.querySelectorAll('button');
      divs = analogsDiv.querySelectorAll('#analog');
      for (let i = 0; i < numAn; i++) {
        buts[i].setAttribute('data-target', '#analog' + i);
        buts[i].setAttribute('aria-controls', 'analog' + i);
        buts[i].innerText = '(' + String(i + 1) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1); 
        divs[i].setAttribute('id', 'analog' + i);      
      } 
      // Digitals
      for (let i = 0; i < numDg; i++) {
        clon = digitalTemp.content.cloneNode(true);
        digitalsDiv.appendChild(clon); 
      }      
      buts = digitalsDiv.querySelectorAll('button');
      divs = digitalsDiv.querySelectorAll('#digital');
      for (let i = 0; i < numDg; i++) {
        buts[i].setAttribute('data-target', '#digital' + i);
        buts[i].setAttribute('aria-controls', 'digital' + i);
        buts[i].innerText = '(' + String(i + 1 + numAn) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1); 
        divs[i].setAttribute('id', 'digital' + i);      
      }   
      // Modbuses
      for (let i = 0; i < numMo; i++) {
        clon = modbusTemp.content.cloneNode(true);
        modbusesDiv.appendChild(clon); 
      }      
      buts = modbusesDiv.querySelectorAll('button');
      divs = modbusesDiv.querySelectorAll('#modbus');
      for (let i = 0; i < numMo; i++) {
        buts[i].setAttribute('data-target', '#modbus' + i);
        buts[i].setAttribute('aria-controls', 'modbus' + i);
        buts[i].innerText = '(' + String(i + 1 + +numAn + numDg) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1); 
        divs[i].setAttribute('id', 'modbus' + i);      
      }
      // Times
      for (let i = 0; i < numTm; i++) {
        clon = timeTemp.content.cloneNode(true);
        timesDiv.appendChild(clon); 
      }      
      buts = timesDiv.querySelectorAll('button');
      divs = timesDiv.querySelectorAll('#time');
      for (let i = 0; i < numTm; i++) {
        buts[i].setAttribute('data-target', '#time' + i);
        buts[i].setAttribute('aria-controls', 'time' + i);
        buts[i].innerText = '(' + String(i + 1) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1); 
        divs[i].setAttribute('id', 'time' + i);      
      }
      // make id      
      let datas = 
      ['ge_u08','ge_u16','ge_u32','an_u08','an_f32','dg_u08','dg_u16','mo_u08','mo_u16','mo_f32','tm_u08']; 
      for (let i = 0; i < datas.length; i++) {        
        items = mainForm.querySelectorAll('#x' + datas[i]);        
        for (let j = 0; j < items.length; j++) {
          items[j].id += ('0' + j).slice(-2);
          statusDisp.textContent += items[j].id + '\r\n';
        }        
      }
    }
    create();

    loraGetBut.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      
      statusDisp.textContent = 'fgdfgdfgd';
                 
    });
    
    loraSaveBut.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let lora = '';               
      if (loraForm.checkValidity()) {
        statusDisp.textContent = 'validaion ok';
        items = loraForm.querySelectorAll('input,select');
        let val = 0;        
        for (let i = 0; i < items.length; i++) {                      
          if (items[i].type === 'checkbox') {
            val = items[i].checked ? 1 : 0;                    
          } else {
            val = items[i].value;
          }
          lora += items[i].id + val + '\r\n';                                           
        }                                         
      }                  
      statusDisp.textContent = lora;
      //port.send(lora);           
    });
        
    saveBut.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let config = '';          
      if (mainForm.checkValidity()) {
        statusDisp.textContent = 'validaion ok';
        items = mainForm.querySelectorAll('input,select');
        let val = 0;        
        for (let i = 0; i < items.length; i++) {          
          if (items[i].id[0] == 'x') {             
            if (items[i].type === 'checkbox') {
              val = items[i].checked ? 1 : 0;                    
            } else {
              val = Number(items[i].value);
            }
            config += items[i].id + val + '\r\n';                        
          }                                 
        }
        config += 'xsave' + '\r\n';                                 
      }            
      statusDisp.textContent = config;
      //port.send(config);      
    });

    timeBut.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      let d = new Date();
      let t = '';
      t += 'xss' + d.getSeconds() + '\r\n';
      t += 'xmm' + d.getMinutes() + '\r\n';
      t += 'xhh' + d.getHours() + '\r\n';
      t += 'xdd' + d.getDay() + '\r\n';
      t += 'xmh' + String(d.getMonth() + 1) + '\r\n';
      t += 'xyy' + d.getFullYear() + '\r\n';
      t += 'xtime' + '\r\n';
      statusDisp.textContent = t;      
      //port.send(t);      
    });

    function connect() {
      port.connect().then(() => {
        statusDisp.textContent = '';
        connectBut.textContent = 'Disconnect';
        port.onReceive = data => {
          let textDecoder = new TextDecoder();
          console.log(textDecoder.decode(data));
        }
        port.onReceiveError = error => {
          console.error(error);
        };
      }, error => {
        statusDisp.textContent = error;
      });
    }    
    connectBut.addEventListener('click', function() {
      if (port) {
        port.disconnect();
        connectBut.textContent = 'Connect';
        statusDisp.textContent = '';
        port = null;
      } else {
        serial.requestPort().then(selectedPort => {
          port = selectedPort;
          connect();
        }).catch(error => {
          statusDisp.textContent = error;
        });
      }
    }); 
    serial.getPorts().then(ports => {
      if (ports.length == 0) {
        //statusDisp.textContent = 'No device found.';
      } else {
        statusDisp.textContent = 'Connecting...';
        port = ports[0];
        connect();
      }
    });
    
  });
})();

/*    
getBut.addEventListener('click', function() {
  //if (!port) {
  //  return;
  //}           
  statusDisp.textContent = 'deleted';      
  //port.send(t);      
}); 

readBut.addEventListener('click', function() {
  //if (!port) {
  //  return;
  //}           
        
});    

serialport.parsers.readline('\n');
parser: SerialPort.parsers.readline('\r')
var serialPort = new SerialPort("/dev/ttyACM0", {
  autoOpen: false,
  parser: SerialPort.parsers.readline('\n'),
  baudrate:115200
}); 

var myPort = new serialport(portName, {
  baudRate: 9600,
  parser: new serialport.parsers.Readline('\r\n')
});
*/

