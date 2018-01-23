(function($){
    $(function(){
    
    var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
    var ws_waiting = 0;
    
    var r_led = 0;
    var g_led = 0;
    var b_led = 0;
    var active_mode = 0;
    
    var canvas = document.getElementById('status');
    var context = canvas.getContext('2d');
    var centerX = canvas.width / 2;
    var centerY = canvas.height / 2;
    var radius = 7.5;
    
    /* House Keeping */
    drawStatus("blue");
    $("#cslider").prop('disabled', true);
    
    
    // Get Modes as JSON
    $.getJSON( "modes.json", function( data ) {
    var items = [];
    $.each( data.effects, function( key, val ) {
      items.push( "<li><button id='" + key + "'>"); 
    
        items.push(val.name);
    
      items.push("</a></li>" );
    });
    
    $( "<ol/>", {
      "id": "mode_list",
      html: items.join( "" )
    }).appendTo( ".mode" );
    });
    
    // Update colour picker to match current settings
    $.getJSON( "/settings.json", function( data ) {
      
      $.each(data, function (key, val) {
        if(!Array.isArray(key)) {
          switch(key) {
            case "device_ip":
              $("#ip_holder").append(val);
            break;
            case "device_time":
              $("#time_holder").append(val);
            break;
            case "device_software_version":
              $("#version_holder").append(val);
            break;
            case "device_name":
              $("#title_holder").html(val);
            break;
          }
        }
      });
      
      $.each( data.active, function( key, val ) {
        switch(key) {
          case "red":
            r_led = val;
            break;
          case "green":
            g_led = val;
            break;
          case "blue":
            b_led = val;
            break;
           case "mode":
            $("button[id='" + val + "']").each(function () {
              $(this).addClass('current_light');
            });
           break;
        }
      });
      
     document.getElementById('cslider').jscolor.fromRGB(r_led, g_led, b_led);
    });
    
    
    /* Websocket */
    function wsInit() {
      // When connection is established, send date
       connection.onopen = function() {
           connection.send('Connect ' + new Date());
           $("#cslider").prop('disabled', false);
           drawStatus("green");
           $('.switch_1').prop('checked', true); // Checks it
       };
      
       // Output error on browser log
       connection.onerror = function(error) {
           console.log('WebSocket Error ', error);
           drawStatus("red");
           $('.switch_1').prop('checked', false); // Unchecks it
       };
      
       // Log messages from the server
       connection.onmessage = function(e) {
           console.log('Server: ', e.data);
           ws_waiting = 0;
       };
    
        // On Connection loss
        connection.onclose = function(e) {
          drawStatus("orange");
          $('.switch_1').prop('checked', false); // Unchecks it
          alert("Disconnected. Reloading page in 5 seconds.");
          console.log('Disconnected!');
    
          setTimeout(
            function() 
            {
              location.reload();
            }, 5000);
          
       };
    
    
    }
    
    /* ws Commands */
    function wsCommand(cmd){
      console.log("wsCommand: ", cmd);
      
      if(ws_waiting == 0) {
        connection.send(cmd); 
        ws_waiting++;
      } else {
        console.log("ws_waiting != 0");
      }
    
    }
    
    function wsSetColour(hexColour) {
      console.log("wsSetColour: ", hexColour);
      wsCommand(hexColour);
    }
    
    
    /* Methods */
    
    function drawStatus(color) {
        context.clearRect(0, 0, canvas.width, canvas.height);
        context.beginPath();
        context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
        context.fillStyle = color;
        context.fill();
    }
    
    
     function setColour() {
         var rgb = $('.jscolor').val();
         console.log('setColour: ' + rgb);
         wsSetColour(rgb);
     }
    
    function setFX(fx) {
        console.log("Mode: ", fx);
        wsCommand("*" + fx);
    }
    /* UI Handlers */
    $("#cslider").change(function() {
        setColour();
    });
    
    $(document).on('click','#mode_list li button',function(e) {
      setFX(String($(this).attr("id")));
      // https://stackoverflow.com/questions/20179413/add-remove-active-class-for-ul-list-with-jquery
      $('#mode_list li button.current_light').removeClass('current_light');
      $(this).addClass('current_light');
      
    });
    
    $('.switch_1').change(function() {
        // this will contain a reference to the checkbox   
        if (this.checked) {
            // the checkbox is now checked 
            setColour();
        } else {
            // the checkbox is now no longer checked
            wsSetColour("#000000");
        }
    });
    
    wsInit();
    
    }); // end of document ready
    })(jQuery); // end of jQuery name space