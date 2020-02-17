(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let lorawanForm = document.querySelector('#lorawan-form');
    let generalForm = document.querySelector('#general-form'); 
    let timeForm = document.querySelector('#time-form');
    let channelsForm = document.querySelector('#channels-form');

    let lorawanTemp = document.querySelectorAll('template')[0];   
    let generalTemp = document.querySelectorAll('template')[1];
    let timeTemp = document.querySelectorAll('template')[2];
    let analogTemp = document.querySelectorAll('template')[3];
    let digitalTemp = document.querySelectorAll('template')[4];
    let modbusTemp = document.querySelectorAll('template')[5];
    
    let mainDiv = document.querySelector('#main-div');
    let formDiv = document.querySelector('#form-div');
    let buttonDiv = document.querySelector('#button-div');
    let lorawanDiv = document.querySelector('#lorawan-div');
    let generalDiv = document.querySelector('#general-div');
    let timeDiv = document.querySelector('#time-div');
    let analogDiv = document.querySelector('#analog-div');
    let digitalDiv = document.querySelector('#digital-div');
    let modbusDiv = document.querySelector('#modbus-div');
     
    let connectBtn = document.querySelector('#connect-btn');    
    let lorawanBtn = document.querySelector('#lorawan-btn');
    let lorawanGetBtn = document.querySelector('#lorawan-get-btn');
    let lorawanSaveBtn = document.querySelector('#lorawan-save-btn');
    let lorawanBackBtn = document.querySelector('#lorawan-back-btn');
    let generalBtn = document.querySelector('#general-btn');
    let generalGetBtn = document.querySelector('#general-get-btn');
    let generalSaveBtn = document.querySelector('#general-save-btn');
    let generalBackBtn = document.querySelector('#general-back-btn');
    let timeBtn = document.querySelector('#time-btn');
    let timeGetBtn = document.querySelector('#time-get-btn');
    let timeSaveBtn = document.querySelector('#time-save-btn');
    let timeAdjustBtn = document.querySelector('#time-adjust-btn');
    let timeBackBtn = document.querySelector('#time-back-btn');
    let channelsBtn = document.querySelector('#channels-btn'); 
    let channelsGetBtn = document.querySelector('#channels-get-btn');
    let channelsSaveBtn = document.querySelector('#channels-save-btn');
    let channelsFetchBtn = document.querySelector('#channels-fetch-btn');
    let channelsBackBtn = document.querySelector('#channels-back-btn');    
    let port;
    let statusDisp = document.querySelector('#status');

    let numAn = 2, numDg = 2, numMo = 8, numTm = 2;
    let items;
    
    lorawanBtn.addEventListener('click', function() {
      buttonDiv.hidden = true;
      formDiv.hidden = false;
      lorawanForm.hidden = false;
      generalForm.hidden = true;
      timeForm.hidden = true;
      channelsForm.hidden = true;                
    });

    lorawanGetBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      //port.send('at+get_config=lora:status' + '\r\n');                 
    });

    lorawanSaveBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let str = '';               
      if (lorawanForm.checkValidity()) {
        // statusDisp.textContent = 'validaion ok';
        items = lorawanForm.querySelectorAll('input,select');
        let value = 0;        
        for (let i = 0; i < items.length; i++) {                      
          if (items[i].type === 'checkbox') {
            value = items[i].checked ? 1 : 0;                    
          } else {
            value = items[i].value;
          }
          str += 'at+set_config=lora:' + items[i].id + ':' + value + '\r\n';                                           
        }                                         
      }                 
      // statusDisp.textContent = str;
      //port.send(str);           
    });

    lorawanBackBtn.addEventListener('click', function() {
      buttonDiv.hidden = false;
      formDiv.hidden = true;                      
    });

    generalBtn.addEventListener('click', function() {
      buttonDiv.hidden = true;
      formDiv.hidden = false;
      lorawanForm.hidden = true;
      generalForm.hidden = false;
      timeForm.hidden = true;
      channelsForm.hidden = true;                
    });

    generalGetBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      //port.send('xget_ge');      
    });

    generalSaveBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let str = '';          
      if (generalForm.checkValidity()) {
        // statusDisp.textContent = 'validaion ok';
        items = generalForm.querySelectorAll('input,select');
        let value = 0;        
        for (let i = 0; i < items.length; i++) {          
          if (items[i].id[0] == 'x') {             
            if (items[i].type === 'checkbox') {
              value = items[i].checked ? 1 : 0;                    
            } else {
              value = Number(items[i].value);
            }
            str += items[i].id + value + '\r\n';                        
          }                                 
        }
        str += 'xsave' + '\r\n';                                 
      }            
      // statusDisp.textContent = str;
      //port.send(str);      
    });

    generalBackBtn.addEventListener('click', function() {
      buttonDiv.hidden = false;
      formDiv.hidden = true;                      
    });

    timeBtn.addEventListener('click', function() {
      buttonDiv.hidden = true;
      formDiv.hidden = false;
      lorawanForm.hidden = true;
      generalForm.hidden = true;
      timeForm.hidden = false;
      channelsForm.hidden = true;                
    });

    timeGetBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      //port.send('xget_tm');      
    });

    timeSaveBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let str = '';          
      if (timeForm.checkValidity()) {
        // statusDisp.textContent = 'validaion ok';
        items = timeForm.querySelectorAll('input,select');
        let value = 0;        
        for (let i = 0; i < items.length; i++) {          
          if (items[i].id[0] == 'x') {             
            if (items[i].type === 'checkbox') {
              value = items[i].checked ? 1 : 0;                    
            } else {
              value = Number(items[i].value);
            }
            str += items[i].id + value + '\r\n';                        
          }                                 
        }
        str += 'xsave' + '\r\n';                                 
      }            
      // statusDisp.textContent = str;
      //port.send(str);      
    });

    timeAdjustBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      let d = new Date();
      let str = '';
      str += 'xss' + d.getSeconds() + '\r\n';
      str += 'xmm' + d.getMinutes() + '\r\n';
      str += 'xhh' + d.getHours() + '\r\n';
      str += 'xdd' + d.getDate() + '\r\n';
      str += 'xwd' + d.getDay() + '\r\n';
      str += 'xmh' + String(d.getMonth() + 1) + '\r\n';
      str += 'xyy' + d.getFullYear() + '\r\n';
      str += 'xtime' + '\r\n';
      // statusDisp.textContent = str;      
      //port.send(str);      
    });

    timeBackBtn.addEventListener('click', function() {
      buttonDiv.hidden = false;
      formDiv.hidden = true;                      
    });

    channelsBtn.addEventListener('click', function() {
      buttonDiv.hidden = true;
      formDiv.hidden = false;
      lorawanForm.hidden = true;
      generalForm.hidden = true;
      timeForm.hidden = true;
      channelsForm.hidden = false;                
    });

    channelsGetBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      //port.send('xget_ch');      
    });

    channelsSaveBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let str = '';          
      if (channelsForm.checkValidity()) {
        // statusDisp.textContent = 'validaion ok';
        items = channelsForm.querySelectorAll('input,select');
        let value = 0;        
        for (let i = 0; i < items.length; i++) {          
          if (items[i].id[0] == 'x') {             
            if (items[i].type === 'checkbox') {
              value = items[i].checked ? 1 : 0;                    
            } else {
              value = Number(items[i].value);
            }
            str += items[i].id + value + '\r\n';                        
          }                                 
        }
        str += 'xsave' + '\r\n';                                 
      }            
      // statusDisp.textContent = str;
      //port.send(str);      
    });

    channelsFetchBtn.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}      
      //port.send('xfetch');      
    });

    channelsBackBtn.addEventListener('click', function() {
      buttonDiv.hidden = false;
      formDiv.hidden = true;                      
    });    
    
    function create() {
      // statusDisp.textContent = '';
      let clon;
      let btns;
      let divs;
      // LORAWAN
      clon = lorawanTemp.content.cloneNode(true);     
      lorawanDiv.appendChild(clon);                     
      // GENERAL
      clon = generalTemp.content.cloneNode(true);     
      generalDiv.appendChild(clon); 
      // TIME
      for (let i = 0; i < numTm; i++) {
        clon = timeTemp.content.cloneNode(true);
        timeDiv.appendChild(clon); 
      }      
      btns = timeDiv.querySelectorAll('button');
      divs = timeDiv.querySelectorAll('#time');
      for (let i = 0; i < numTm; i++) {
        btns[i].setAttribute('data-target', '#time' + i);
        btns[i].setAttribute('aria-controls', 'time' + i);
        btns[i].textContent = 'Time ' + String(i + 1); 
        divs[i].setAttribute('id', 'time' + i);      
      }      
      // ANALOG
      for (let i = 0; i < numAn; i++) {
        clon = analogTemp.content.cloneNode(true);
        analogDiv.appendChild(clon); 
      }      
      btns = analogDiv.querySelectorAll('button');
      divs = analogDiv.querySelectorAll('#analog');
      for (let i = 0; i < numAn; i++) {
        btns[i].setAttribute('data-target', '#analog' + i);
        btns[i].setAttribute('aria-controls', 'analog' + i);        
        btns[i].textContent = '#' + String(i + 1) + ' (Analog ' + String(i + 1) + ')' + ' Value: 0'; 
        divs[i].setAttribute('id', 'analog' + i);      
      } 
      // DIGITAL
      for (let i = 0; i < numDg; i++) {
        clon = digitalTemp.content.cloneNode(true);
        digitalDiv.appendChild(clon); 
      }      
      btns = digitalDiv.querySelectorAll('button');
      divs = digitalDiv.querySelectorAll('#digital');
      for (let i = 0; i < numDg; i++) {
        btns[i].setAttribute('data-target', '#digital' + i);
        btns[i].setAttribute('aria-controls', 'digital' + i);
        btns[i].textContent = '#' + String(i + 1 + numAn) + ' (Digital ' + String(i + 1) + ')' + ' Value: 0'; 
        divs[i].setAttribute('id', 'digital' + i);      
      }   
      // MODBUS
      for (let i = 0; i < numMo; i++) {
        clon = modbusTemp.content.cloneNode(true);
        modbusDiv.appendChild(clon); 
      }      
      btns = modbusDiv.querySelectorAll('button');
      divs = modbusDiv.querySelectorAll('#modbus');
      for (let i = 0; i < numMo; i++) {
        btns[i].setAttribute('data-target', '#modbus' + i);
        btns[i].setAttribute('aria-controls', 'modbus' + i);
        btns[i].textContent = '#' + String(i + 1 + +numAn + numDg) + ' (Modbus ' + String(i + 1) + ')' + ' Value: 0'; 
        divs[i].setAttribute('id', 'modbus' + i);      
      }      
      // make id      
      let datas = 
      ['ge_u08','ge_u16','ge_u32','an_u08','an_f32','dg_u08','dg_u16','mo_u08','mo_u16','mo_f32','tm_u08']; 
      for (let i = 0; i < datas.length; i++) {        
        items = formDiv.querySelectorAll('#x' + datas[i]);        
        for (let j = 0; j < items.length; j++) {
          items[j].id += ('0' + j).slice(-2);
          // statusDisp.textContent += items[j].id + '\r\n';
        }        
      }
    }
    create();

    function connect() {
      port.connect().then(() => {
        // statusDisp.textContent = '';
        connectBtn.textContent = 'Disconnect';
        port.onReceive = data => {
          let textDecoder = new TextDecoder();
          console.log(textDecoder.decode(data));
          // here readline parser, and trim
          let dataline;         

          let item;
          let value;          
          let split;
          let n = Number(dataline.slice(7, 9));;
          let btns = channelsForm.querySelectorAll('button');
          if (dataline.startsWith('xan_val')) {
            n = Number(dataline.slice(7, 9));
            value = dataline.slice(9);
            split = btns[n].textContent.split('Value: ');
            btns[n].textContent = btns[n].textContent.replace(split[1], value);
          } else if (dataline.startsWith('xdg_val')) {
            n = Number(dataline.slice(7, 9)) + numAn;
            value = dataline.slice(9);
            split = btns[n].textContent.split('Value: ');
            btns[n].textContent = btns[n].textContent.replace(split[1], value);
          } else if (dataline.startsWith('xmo_val')) { 
            n = Number(dataline.slice(7, 9)) + numAn + numDg;
            value = dataline.slice(9);
            split = btns[n].textContent.split('Value: ');
            btns[n].textContent = btns[n].textContent.replace(split[1], value);          
          } else {
            if (dataline.startsWith('DevEui: ')) {
              item = loraForm.querySelector('#dev-eui');
              value = dataline.slice(8);                        
            } else if (dataline.startsWith('AppEui: ')) {
              item = loraForm.querySelector('#app-eui');
              value = dataline.slice(8);            
            } else if (dataline.startsWith('AppKey: ')) {
              item = loraForm.querySelector('#app-key');
              value = dataline.slice(8);            
            } else if (dataline.startsWith('x')) {
              item = mainForm.querySelector('#' + dataline.slice(0, 9));
              value = Number(dataline.slice(9));            
            }
            if (item.type === 'checkbox') {
              item.checked = value ? true : false;                    
            } else {
              item.value = value;
            } 
          }
        }
        port.onReceiveError = error => {
          console.error(error);
        };
      }, error => {
        // statusDisp.textContent = error;
      });
    }    
    connectBtn.addEventListener('click', function() {
      if (port) {
        port.disconnect();
        connectBtn.textContent = 'Connect';
        // statusDisp.textContent = '';
        port = null;
      } else {
        serial.requestPort().then(selectedPort => {
          port = selectedPort;
          connect();
        }).catch(error => {
          // statusDisp.textContent = error;
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
serialport.parsers.readline('\n');
parser: SerialPort.parsers.readline('\r')
var serialPort = new SerialPort('/dev/ttyACM0", {
  autoOpen: false,
  parser: SerialPort.parsers.readline('\n'),
  baudrate:115200
}); 

var myPort = new serialport(portName, {
  baudRate: 9600,
  parser: new serialport.parsers.Readline('\r\n')
});
*/

