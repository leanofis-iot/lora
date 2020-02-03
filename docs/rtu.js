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

    let numAn = 2, numDg = 2, numMo = 8, numTm = 2;
    let numgeub = 6, numgeus = 2;
    let items;
    let item;
    
    function create() {
      statusDisp.textContent = "";
      let clon;
      let buts;
      let divs;
              
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
        buts[i].innerText = '(' + String(i + 1) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1); 
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
        buts[i].innerText = '(' + String(i + 1) + ')' + '  ' + buts[i].innerText + ' ' + String(i + 1); 
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
      
      let datas = 
      ['ge_ub','ge_us','an_ub','an_us','an_ff','dg_ub','dg_us','mo_ub','mo_us','mo_ff','tm_ub']; 
      for (let i = 0; i < datas.length; i++) {        
        items = mainForm.querySelectorAll('#x' + datas[i]);        
        for (let j = 0; j < items.length; j++) {
          items[j].id += ('0' + j).slice(-2);
          statusDisp.textContent += items[j].id + '\r\n';
        }        
      }

      /*    
      for (let i = 1; i < numAn; i++) {
        item = analogsDiv.querySelector('#analog' + i);
        
        
        items = item.querySelectorAll('[id^="xan_ub"]');
        statusDisp.textContent += items.length + '\r\n';
        
        for (let j = 0; j < items.length; j++) {
          let id = Number(items[j].id.slice(-2)) + i * numgeub;
          items[j].id = items[j].id.slice(0, 6) + id;
          statusDisp.textContent += 'qqqqq' + '\r\n'; 
        }
        
           
      } 
      */

      //
      /*  
      for (let j = 0; j < items.length; j++) {
        //let id = Number(items[j].id.slice(-2)) + i * numgeub;
        //items[j].id = items[j].id.slice(0, 6) + id;
        statusDisp.textContent += 'hrrrrrrrr' + '\r\n';
      }
      */
      /*
      items = clon.querySelectorAll('[id^="xge_us"]');
      for (let j = 0; j < items.length; j++) {
        let id = Number(items[j].id.slice(-2)) + i * numgeus;
        items[j].id = items[j].id.slice(0, 6) + id;
        statusDisp.textContent += items[j].id + '\r\n';
      }
      */
      /*           
      items = analogsDiv.querySelectorAll('input,select');
      for (let i = 0; i < numAn; i++) {
        
          
            items[id].id = 'xan' + ('00' +  String(j + i * numAnBytes)).slice (-3);            
            statusDisp.textContent += items[id].id + '\r\n';
            id++;
                 
        
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
            items[id].id = 'xdg' + ('00' +  String(j + i * numDgBytes)).slice (-3);            
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
      skipindex = [5,10,12,14,15,16,18,19,20,22];      
      items = modbusesDiv.querySelectorAll('input,select');
      id = 0;
      for (let i = 0; i < numMo; i++) {
        for (let j = 0; j < numMoBytes; j++) {
          if (skipindex.indexOf(j) < 0) {
            items[id].id = 'xmo' + ('00' +  String(j + i * numMoBytes)).slice (-3);            
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
          items[id].id = 'xtm' + ('00' +  String(j + i * numTmBytes)).slice (-3);            
          statusDisp.textContent += items[id].id + '\r\n';
          id++;                 
        } 
      } 
      */                 
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
          if (items[i].id[0] == 'x') {             
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
      t += 'xmo' + String(d.getMonth() + 1) + '\r\n';
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

