(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let configurationForm = document.querySelector('#configuration');
    let statusDisplay = document.querySelector('#status');
    let trgAddButton = document.querySelector("#trg-add");    
    let connectButton = document.querySelector("#connect");    
    let timeButton = document.querySelector('#time');
    let readButton = document.querySelector('#read');
    let saveButton = document.querySelector('#save');
    let trgList = document.querySelector("#trg-list");       
    let port;

    trgList.addEventListener('change', function(e) {       
      if (e.target.name == 'trg-inp') { 
        let temp = document.querySelectorAll('template')[Number(e.target.value) - 1]; 
        let clon = temp.content.cloneNode(true);       
        e.target.parentNode.replaceWith(clon);                    
      }
    });
    trgList.addEventListener('click', function(e) {       
      if (e.target.name == 'trg-del') {
        //e.target.closest('li').remove();
        e.target.parentNode.remove();
        let lis = trgList.querySelectorAll('li');
        if (lis.length < 99) {
          trgAddButton.disabled = false;
        }
      }      
    });
    trgAddButton.addEventListener('click', function() {
      let temp = document.querySelectorAll('template')[0]; 
      let clon = temp.content.cloneNode(true);     
      trgList.appendChild(clon);      
      statusDisplay.textContent = '';
      let lis = trgList.querySelectorAll('li');      
      for (let i = 0; i < lis.length; i++) {
        let els = lis[i].querySelectorAll('input,select');
        for (let j = 0; j < els.length; j++) {
          els[j].id = '&alr_b' + ('00' +  String(j + i * 6)).slice (-3);
          statusDisplay.textContent += els[j].id;
        }
      }
      if (lis.length >= 99) {
        this.disabled = true;
      }
    }); 
    
    saveButton.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let config = '';      
      if (configurationForm.checkValidity()) {
        statusDisplay.textContent = 'validaion ok';
        let els = configurationForm.querySelectorAll('input,select');
        for (let i = 0; i < els.length; i++) {
          if (els[i].id[0] == '&') {
            let val = 0;
            if (els[i].id[5] == 'b') { 
              if (els[i].type === 'checkbox') {
                val = els[i].checked ? 1 : 0;            
              } else {
                val = parseInt(els[i].value);
              }  
            } else if (els[i].id[5] == 'w') {
              val = parseInt(els[i].value);
            } else if (els[i].id[5] == 'f') {
              val = parseFloat(els[i].value);
            }
            config += els[i].id + val + '\r\n';
          }                     
        }
        config += '&save' + '\r\n';                  
      }      
      statusDisplay.textContent = config;      
      //port.send(config);      
    });

    readButton.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //} 
      trgList.innerHTML = '';    
      
      statusDisplay.textContent = 'deleted';      
      //port.send(t);      
    });

    timeButton.addEventListener('click', function() {
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
      statusDisplay.textContent = t;      
      //port.send(t);      
    });

    function connect() {
      port.connect().then(() => {
        statusDisplay.textContent = '';
        connectButton.textContent = 'Disconnect';

        port.onReceive = data => {
          let textDecoder = new TextDecoder();
          console.log(textDecoder.decode(data));
        }
        port.onReceiveError = error => {
          console.error(error);
        };
      }, error => {
        statusDisplay.textContent = error;
      });
    }    
    
    connectButton.addEventListener('click', function() {
      if (port) {
        port.disconnect();
        connectButton.textContent = 'Connect';
        statusDisplay.textContent = '';
        port = null;
      } else {
        serial.requestPort().then(selectedPort => {
          port = selectedPort;
          connect();
        }).catch(error => {
          statusDisplay.textContent = error;
        });
      }
    });     

    serial.getPorts().then(ports => {
      if (ports.length == 0) {
        statusDisplay.textContent = 'No device found.';
      } else {
        statusDisplay.textContent = 'Connecting...';
        port = ports[0];
        connect();
      }
    });
  });
})();

/*
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

/*
saveButton.addEventListener('click', function() {
  //if (!port) {
  //  return;
  //}
  let config = '';      
  if (configurationForm.checkValidity()) {
    statusDisplay.textContent = 'heyyy';
    for (let i = 0; i < 2; i++) {
      let ii = '&lrb';        
      ii += ('0' + i).slice (-2);      
      let lrb = parseInt(document.getElementById(ii).value);
      config += ii + lrb + '\r\n';
    }
    for (let i = 0; i < 1; i++) {
      let ii = '&lrw';        
      ii += ('0' + i).slice (-2);      
      let lrw = parseInt(document.getElementById(ii).value);
      config += ii + lrw + '\r\n';
    }
    let tmb = 0;                  
    for (let i = 0; i < 10; i++) {
      let ii = '&tmb';
      ii += ('0' + i).slice (-2);
      if (document.getElementById(ii).type === 'checkbox') { // if id ii = trg-upl
        tmb = document.getElementById(ii).checked ? 1 : 0;            
      } else {
        tmb = parseInt(document.getElementById(ii).value);
      }           
      config += ii + tmb + '\r\n';
    }
    for (let i = 0; i < 2; i++) {
      let ii = '&rlw';        
      ii += ('0' + i).slice (-2);      
      let rlw = parseInt(document.getElementById(ii).value);
      config += ii + rlw + '\r\n';
    }
    let anb = 0;                  
    for (let i = 0; i < 26; i++) {
      let ii = '&anb';
      ii += ('0' + i).slice (-2);
      if (document.getElementById(ii).type === 'checkbox') {
        anb = document.getElementById(ii).checked ? 1 : 0;            
      } else {
        anb = parseInt(document.getElementById(ii).value);
      }           
      config += ii + anb + '\r\n';
    }
    for (let i = 0; i < 16; i++) {
      let ii = '&anf';        
      ii += ('0' + i).slice (-2);      
      let anf = parseFloat(document.getElementById(ii).value);
      config += ii + anf + '\r\n';
    }
    let dgb = 0;                  
    for (let i = 0; i < 12; i++) {
      let ii = '&dgb';
      ii += ('0' + i).slice (-2);
      if (document.getElementById(ii).type === 'checkbox') {
        dgb = document.getElementById(ii).checked ? 1 : 0;            
      } else {
        dgb = parseInt(document.getElementById(ii).value);
      }           
      config += ii + dgb + '\r\n';
    }        
  }      
  statusDisplay.textContent = config;      
  //port.send(config);      
});
*/
/*
let els = document.body.querySelectorAll('input,select');
for (let i = 0; i < els.length; i++) {
  if (els[i].id[0] == '&') {
    if (els[i].id[5] == 'b') {              
    } else if (els[i].id[5] == 'w') {
    } else if (els[i].id[5] == 'f') {
    }
  }          
}
*/
