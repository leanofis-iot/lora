(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let mainForm = document.querySelector('#main-form');    
    let generalTemp = document.querySelectorAll('template')[0];
    let analogTemp = document.querySelectorAll('template')[1];
    let digitalTemp = document.querySelectorAll('template')[2];
    let modbusTemp = document.querySelectorAll('template')[3];
    let timeTemp = document.querySelectorAll('template')[4];
    let generalsDiv = document.querySelector("#generals");
    let analogsDiv = document.querySelector("#analogs");
    let digitalsDiv = document.querySelector("#digitals");
    let modbusesDiv = document.querySelector("#modbuses");
    let timesDiv = document.querySelector("#times");           
    let connectBut = document.querySelector("#connect");    
    let timeBut = document.querySelector('#set-time');
    let readBut = document.querySelector('#read');
    let saveBut = document.querySelector('#save');     
    let port;
    let statusDisp = document.querySelector('#status');

    let numAn = 2, numDg = 2, numMo = 8, numTm = 2, numGenBytes = 10, numAnBytes = 38, numDgBytes = 13, numMoBytes = 49, numTmBytes = 5;
    let items;
    
    function create() {
      statusDisp.textContent = "";
      let clon;
      let buts;
      let divs;
      let skipindex = [];      
      let id;  
      // Generals
      clon = generalTemp.content.cloneNode(true);     
      generalsDiv.appendChild(clon);      
      skipindex = [4,6];      
      items = generalsDiv.querySelectorAll('input,select');
      id = 0;
      for (let j = 0; j < numGenBytes; j++) {
        if (skipindex.indexOf(j) < 0) {
          items[id].id = '&ge' + ('00' +  String(j)).slice (-3);            
          statusDisp.textContent += items[id].id + '\r\n';
          id++;
        }        
      }
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
      skipindex = [4,5,6,8,9,10,12,13,14,16,17,18,20,21,22,24,25,26,28];      
      items = analogsDiv.querySelectorAll('input,select');
      id = 0;
      for (let i = 0; i < numAn; i++) {
        for (let j = 0; j < numAnBytes; j++) {
          if (skipindex.indexOf(j) < 0) {
            items[id].id = '&an' + ('00' +  String(j + i * numAnBytes)).slice (-3);            
            statusDisp.textContent += items[id].id + '\r\n';
            id++;
          }        
        } 
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
        buts[i].innerText = '(' + String(i + 3) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1);
        divs[i].setAttribute('id', 'digital' + i);      
      }            
      skipindex = [3];      
      items = digitalsDiv.querySelectorAll('input,select');
      id = 0;
      for (let i = 0; i < numDg; i++) {
        for (let j = 0; j < numDgBytes; j++) {
          if (skipindex.indexOf(j) < 0) {
            items[id].id = '&dg' + ('00' +  String(j + i * numDgBytes)).slice (-3);            
            statusDisp.textContent += items[id].id + '\r\n';
            id++;
          }        
        } 
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
        buts[i].innerText = '(' + String(i + 5) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1);
        divs[i].setAttribute('id', 'modbus' + i);      
      }            
      skipindex = [4,5,6,8,9,10,12,13,14,16,17,18,22,27,29,31,32,33,35,36,37,39];      
      items = modbusesDiv.querySelectorAll('input,select');
      id = 0;
      for (let i = 0; i < numMo; i++) {
        for (let j = 0; j < numMoBytes; j++) {
          if (skipindex.indexOf(j) < 0) {
            items[id].id = '&mo' + ('00' +  String(j + i * numMoBytes)).slice (-3);            
            statusDisp.textContent += items[id].id + '\r\n';
            id++;
          }        
        } 
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
        buts[i].innerText += ' ' + String(i + 1);
        divs[i].setAttribute('id', 'time' + i);      
      }           
      items = timesDiv.querySelectorAll('input,select');
      id = 0;
      for (let i = 0; i < numTm; i++) {        
        for (let j = 0; j < numTmBytes; j++) {         
          items[id].id = '&tm' + ('00' +  String(j + i * numTmBytes)).slice (-3);            
          statusDisp.textContent += items[id].id + '\r\n';
          id++;                 
        } 
      }                  
    }
    create();    
        
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
          if (items[i].id[0] == '&') {             
            if (items[i].type === 'checkbox') {
              val = items[i].checked ? 1 : 0; 
            } else if (items[i].name == 'coil-mask') {
              val = parseInt(items[i].value, 2);           
            } else {
              val = Number(items[i].value);
            }
            config += items[i].id + val + '\r\n';                        
          }                                 
        }
        config += '&save' + '\r\n';                                 
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
      t += '&ss' + d.getSeconds() + '\r\n';
      t += '&mm' + d.getMinutes() + '\r\n';
      t += '&hh' + d.getHours() + '\r\n';
      t += '&dd' + d.getDay() + '\r\n';
      t += '&mo' + String(d.getMonth() + 1) + '\r\n';
      t += '&yy' + d.getFullYear() + '\r\n';
      t += '&time' + '\r\n';
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
readBut.addEventListener('click', function() {
  //if (!port) {
  //  return;
  //}           
  statusDisp.textContent = 'deleted';      
  //port.send(t);      
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

