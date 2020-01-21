(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let configurationForm = document.getElementById('configuration');
    let statusDisplay = document.getElementById('status');
    let connectButton = document.getElementById("connect");    
    let timeButton = document.getElementById('time');
    let readButton = document.getElementById('read');
    let saveButton = document.getElementById('save');
    let port;

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

    saveButton.addEventListener('click', function() {
      //if (!port) {
      //  return;
      //}
      let config = '';      
      if (configurationForm.checkValidity()) {
        statusDisplay.textContent = 'heyyy';
        let b = 0;                  
        for (let i = 0; i < 50; i++) {
          let ii = '&b';
          ii += ('0' + i).slice (-2);
          if (document.getElementById(ii).type === 'checkbox') {
            b = document.getElementById(ii).checked ? 1 : 0;            
          } else {
            b = parseInt(document.getElementById(ii).value);
          }           
          config += ii + b + '\r\n';
        }
        for (let i = 0; i < 3; i++) {
          let ii = '&w';        
          ii += ('0' + i).slice (-2);      
          let w = parseInt(document.getElementById(ii).value);
          config += ii + w + '\r\n';
        }
        for (let i = 0; i < 16; i++) {
          let ii = '&f';        
          ii += ('0' + i).slice (-2);      
          let f = parseFloat(document.getElementById(ii).value);
          config += ii + f + '\r\n';
        }
      }      
      statusDisplay.textContent = config;      
      //port.send(config);      
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
