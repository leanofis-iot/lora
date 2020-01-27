(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let configurationForm = document.getElementById('configuration');
    let statusDisplay = document.getElementById('status');
    let trgAddButton = document.getElementById("trg-add");    
    let connectButton = document.getElementById("connect");    
    let timeButton = document.getElementById('time');
    let readButton = document.getElementById('read');
    let saveButton = document.getElementById('save');
    let trgActList = document.getElementById("trg-act");       
    let port;

    trgActList.addEventListener('change', function(e) {       
      if (e.target.id == 'trg-del') {
        //e.target.closest('li').remove();
        e.target.parentNode.remove();
      } else if (e.target.id == 'trg-inp') { 
        let temp = document.getElementsByTagName("template")[Number(e.target.value) - 1]; 
        let clon = temp.content.cloneNode(true);       
        e.target.parentNode.replaceWith(clon);                    
      }
    });
    trgAddButton.addEventListener('click', function() {
      let temp = document.getElementsByTagName("template")[0]; 
      let clon = temp.content.cloneNode(true);     
      trgActList.appendChild(clon);
    }); 
    
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
