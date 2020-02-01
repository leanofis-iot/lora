(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let mainF = document.querySelector('#main-form');
    let statusD = document.querySelector('#status');
    let channelTemp = document.querySelectorAll('template')[0]; 
    let channelIfttTemp = document.querySelectorAll('template')[1]; 
    let timeIfttTemp = document.querySelectorAll('template')[2]; 
    let addChannelB = document.querySelector("#add-channel");
    let addChannelIfttB = document.querySelector("#add-channel-iftt");
    let addTimeIfttB = document.querySelector("#add-time-iftt");        
    let connectB = document.querySelector("#connect");    
    let timeB = document.querySelector('#time');
    let readB = document.querySelector('#read');
    let saveB = document.querySelector('#save');
    let channelL = document.querySelector("#channel-list");
    let channelIfttL = document.querySelector("#channel-iftt-list");
    let timeIfttL = document.querySelector("#time-iftt-list");       
    let port;

    addChannelB.addEventListener('click', function() {
      let clon = channelTemp.content.cloneNode(true);     
      channelL.appendChild(clon);      
      statusD.textContent = '';
      let skipindex = [6,7,8,10,11,12,14,15,16,18,19,20];
      let lis = channelL.querySelectorAll('li');      
      for (let i = 0; i < lis.length; i++) {
        let els = lis[i].querySelectorAll('input,select');
        for (let j = 0; j < els.length; j++) {
          if (skipindex.indexOf(j) < 0) {
            els[j].id = '&ch' + ('00' +  String(j + i * 29)).slice (-3);
            statusD.textContent += els[j].id;
          }          
        }
      }
      //if (lis.length >= 99) {
      //  this.disabled = true;
      //}
    }); 

    addChannelIfttB.addEventListener('click', function() {
      let clon = channelIfttTemp.content.cloneNode(true);     
      channelIfttL.appendChild(clon);      
      statusD.textContent = '';
      let skipindex = [3,4,5,7,8,9,11,13];
      let lis = channelIfttL.querySelectorAll('li');      
      for (let i = 0; i < lis.length; i++) {
        let els = lis[i].querySelectorAll('input,select');
        for (let j = 0; j < els.length; j++) {
          if (skipindex.indexOf(j) < 0) {
            els[j].id = '&if' + ('00' +  String(j + i * 29)).slice (-3);
            statusD.textContent += els[j].id;
          }          
        }
      }
      //if (lis.length >= 99) {
      //  this.disabled = true;
      //}
    }); 

    addTimeIfttB.addEventListener('click', function() {
      let clon = timeIfttTemp.content.cloneNode(true);     
      timeIfttL.appendChild(clon);      
      statusD.textContent = '';      
      let lis = timeIfttL.querySelectorAll('li');      
      for (let i = 0; i < lis.length; i++) {
        let els = lis[i].querySelectorAll('input,select');
        for (let j = 0; j < els.length; j++) {          
          els[j].id = '&if' + ('00' +  String(j + i * 29)).slice (-3);
          statusD.textContent += els[j].id;                    
        }
      }
      //if (lis.length >= 99) {
      //  this.disabled = true;
      //}
    }); 

    
/*
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
    
    
    saveB.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let config = '';      
      if (mainF.checkValidity()) {
        statusD.textContent = 'validaion ok';
        let els = mainF.querySelectorAll('input,select');
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
      statusD.textContent = config;      
      //port.send(config);      
    });

    readB.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //} 
      trgList.innerHTML = '';    
      
      statusD.textContent = 'deleted';      
      //port.send(t);      
    });

    timeB.addEventListener('click', function() {
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
      statusD.textContent = t;      
      //port.send(t);      
    });

    */

    function connect() {
      port.connect().then(() => {
        statusD.textContent = '';
        connectB.textContent = 'Disconnect';

        port.onReceive = data => {
          let textDecoder = new TextDecoder();
          console.log(textDecoder.decode(data));
        }
        port.onReceiveError = error => {
          console.error(error);
        };
      }, error => {
        statusD.textContent = error;
      });
    }    
    
    connectB.addEventListener('click', function() {
      if (port) {
        port.disconnect();
        connectB.textContent = 'Connect';
        statusD.textContent = '';
        port = null;
      } else {
        serial.requestPort().then(selectedPort => {
          port = selectedPort;
          connect();
        }).catch(error => {
          statusD.textContent = error;
        });
      }
    });     

    serial.getPorts().then(ports => {
      if (ports.length == 0) {
        statusD.textContent = 'No device found.';
      } else {
        statusD.textContent = 'Connecting...';
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
saveB.addEventListener('click', function() {
  //if (!port) {
  //  return;
  //}
  let config = '';      
  if (mainF.checkValidity()) {
    statusD.textContent = 'heyyy';
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
  statusD.textContent = config;      
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
