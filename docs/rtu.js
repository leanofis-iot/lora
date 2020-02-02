(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let mainForm = document.querySelector('#main-form');    
    let generalTemp = document.querySelectorAll('template')[0];
    let channelTemp = document.querySelectorAll('template')[1];
    let timeTemp = document.querySelectorAll('template')[2];
    let generalsDiv = document.querySelector("#generals");
    let channelsDiv = document.querySelector("#channels");
    let timesDiv = document.querySelector("#times");           
    let connectBut = document.querySelector("#connect");    
    let timeBut = document.querySelector('#set-time');
    let readBut = document.querySelector('#read');
    let saveBut = document.querySelector('#save');     
    let port;
    let statusDisp = document.querySelector('#status');

    let numCh = 12, numTm = 2, numGenBytes = 10, numChBytes = 51, numTmBytes = 5;
    let items;
    let query;

    function create() {
      statusDisp.textContent = "";
      let clon;
      let buts;
      let divs;
      let skipindex = [];      
      let id;  

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

      for (let i = 0; i < numCh; i++) {
        clon = channelTemp.content.cloneNode(true);        
        channelsDiv.appendChild(clon); 
      } 
      buts = channelsDiv.querySelectorAll('button');
      divs = channelsDiv.querySelectorAll('#channel');
      for (let i = 0; i < numCh; i++) {
        buts[i].setAttribute('data-target', '#channel' + i);
        buts[i].setAttribute('aria-controls', 'channel' + i);
        buts[i].innerText += ' ' + String(i + 1); 
        divs[i].setAttribute('id', 'channel' + i);      
      }            
      skipindex = [6,7,8,10,11,12,14,15,16,18,19,20,24,29,31,33,34,35,37,38,39,41];      
      items = channelsDiv.querySelectorAll('input,select');
      id = 0;
      for (let i = 0; i < numCh; i++) {
        for (let j = 0; j < numChBytes; j++) {
          if (skipindex.indexOf(j) < 0) {
            items[id].id = '&ch' + ('00' +  String(j + i * numChBytes)).slice (-3);            
            statusDisp.textContent += items[id].id + '\r\n';
            id++;
          }        
        } 
      } 

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

    channelsDiv.addEventListener('change', function(e) {             
      if (e.target.name == 'input') { 
        items = e.target.parentNode.querySelectorAll('*');
        for (let i = 0; i < items.length; i++) {
          items[i].hidden = false; 
        }        
        if (e.target.value == '0') {            
          items = e.target.parentNode.querySelectorAll(
            '[name="slave"],[name="function"],[name="register"],[name="type"],[name="quantity"],' +
            '[name="decimal"],[name="coil-mask"],[name="discrete-mask"],' + 
            '[for="slave"],[for="function"],[for="register"],[for="type"],[for="quantity"],' +
            '[for="decimal"],[for="coil-mask"],[for="discrete-mask"]'
          );
          for (let i = 0; i < items.length; i++) {
            items[i].hidden = true; 
          }                                
        } else if (e.target.value == '1') {
          items = e.target.parentNode.querySelectorAll(
            '[name="div-scale"],[name="in-min"],[name="in-max"],[name="out-min"],[name="out-max"],' +
            '[name="low-set"],[name="high-set"],[name="slave"],[name="function"],[name="register"],' +
            '[name="type"],[name="quantity"],[name="decimal"],[name="coil-mask"],[name="discrete-mask"],' +             
            '[for="scale"],[for="in-min"],[for="in-max"],[for="out-min"],[for="out-max"],' +
            '[for="low-set"],[for="high-set"],[for="slave"],[for="function"],[for="register"],' +
            '[for="type"],[for="quantity"],[for="decimal"],[for="coil-mask"],[for="discrete-mask"]'
          );
          for (let i = 0; i < items.length; i++) {
            items[i].hidden = true; 
          } 
        } else if (e.target.value == '2') {
          items = e.target.parentNode.querySelectorAll(
            '[name="no"],' +
            '[for="no"]'
          );
          for (let i = 0; i < items.length; i++) {
            items[i].hidden = true; 
          }       
        }                         
      }
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
    trgList.addEventListener('change', function(e) {       
      if (e.target.name == 'trg-inp') { 
        let temp = document.querySelectorAll('template')[Number(e.target.value) - 1]; 
        let clon = temp.content.cloneNode(true);       
        e.target.parentNode.replaceWith(clon);                    
      }
    });   

    readBut.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //} 
      trgList.innerHTML = '';      
      statusDisp.textContent = 'deleted';      
      //port.send(t);      
    });    

    channelL.addEventListener('click', function(e) {       
      if (e.target.name == 'delete') {
        //e.target.closest('li').remove();
        e.target.parentNode.remove();
        //let item = this.querySelectorAll('li');
        //if (item.length < 99) {
        //  addChannelB.disabled = false;
        //}
      } else if (e.target.name == 'hide') {
        e.target.parentNode.hidden = true;
      }     
    });
    
    channelIfttL.addEventListener('click', function(e) {       
      if (e.target.name == 'delete') {
        //e.target.closest('li').remove();
        e.target.parentNode.remove();
        //let item = this.querySelectorAll('li');
        //if (item.length < 99) {
        //  addChannelIfttB.disabled = false;
        //}
      }      
    });    

    timeIfttL.addEventListener('click', function(e) {       
      if (e.target.name == 'delete') {
        //e.target.closest('li').remove();
        e.target.parentNode.remove();
        let item = this.querySelectorAll('li');
        if (item.length < 2) {
          addTimeIfttB.disabled = false;
        }
      }      
    });

    
  */

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

