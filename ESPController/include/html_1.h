const char FILE_INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width=device-width,initial-scale=1.0">
<link href="https://fonts.googleapis.com/css?family=Nova+Mono" rel="stylesheet">
<title>DIY BMS CONTROLLER v4</title>
<!-- jquery-3.3.1.min.js -->
<script src="jquery.js" integrity="sha384-tsQFqpEReu7ZLhBV2VZlAu7zcOV+rXbYlF2cqB8txI/8aZajjp4Bqd+V6D5IgvKT" crossorigin="anonymous"></script>
<script src="echarts.simple.min.js" integrity="" crossorigin="anonymous"></script>
<style>
*{box-sizing:border-box}
body{margin:0;font-family:Arial,Helvetica,sans-serif;}
.header{overflow:hidden;background-color:#d4f0fb;padding:20px 10px}
.header a{float:left;color:#000;text-align:center;padding:12px;text-decoration:none;font-size:18px;line-height:25px;border-radius:4px}
.header a.logo{font-size:25px;font-weight:700}
.header a:hover{background-color:#ddd;color:#000}
.header a.active{background-color:#1e90ff;color:#fff}
.header-right{float:right}
.logo { float:left; background-image: url("logo.gif");width:191px;height:48px;}
.info {width:100%%;}
.info .stat {display:block;width:10%%;border:solid 2px gray;padding:2px;margin:2px;float:left;min-width:175px;}
.info .stat .x {display:block;width:100%%;text-align:right;}
.info .stat .t {font-size:10pt;color:darkgray;}
.info .stat .v {font-size:22pt;font-family: 'Nova Mono', monospace;font-weight:bold;color:#1e90ff;}
#refreshbar { width:100%%; padding:0; margin:0; height:4px; background-color:#d3f9fa;}
.graphs {height:768px;width:100%%; }
.ct-series-a .ct-bar {
  stroke: purple;
  stroke-width: 25px;
}
form > div > div { padding-bottom:8px;}
form label {width:220px;display:inline-block;text-align:right;}
form input {width:100px;display:inline-block;padding-left:10px;padding-right:10px;margin-left:10px;font-family:'Nova Mono',monospace;}
form input[type='input'] {width:225px;}
form input[type='submit'] {font-family:Arial,Helvetica,sans-serif;border: none;color: white;background-color:#1e90ff;text-align:center;text-decoration:none;display:inline-block;font-size:14px;width:150pt;padding:10px;}

@media screen and (max-width:500px){
  .header a,.header-right{float:none}
  .header a{display:block;text-align:left}
  .graphs {height:320px;}
}

@media screen and (max-width:640px){
  .graphs {height:500px;}
}

.error { color:#D8000C;background-color:#FFBABA; padding:10px;display:none; width:100%%;}
.success { color:#000000;background-color:#49BE3B; padding:10px;display:none; width:100%%;}
.page {clear:both; width:100%%; display:none;}

#settingsTable tbody td {min-width:100px;font-family: 'Nova Mono', monospace;text-align:right;}
#settingsTable th {text-align:right;}
#settingsTable .selected {background-color:#d3f9fa;}
</style>
</head>
<body>
<div class="header">
  <div class="logo">&nbsp;</div>
  <div class="header-right">
    <a id="home" class="active" href="#home">Home</a>
    <a id="settings" href="#settings">Settings</a>
    <a id="integration" href="#integration">Integration</a>
    <a id="about" href="#about">About</a>
  </div>
</div>
<div id='refreshbar'></div>
<div id='commserr' class='error'>The controller is having difficulty communicating with the cell monitoring modules.</div>
<div id='iperror' class='error'>Cannot communicate with the controller for status updates.</div>
<div id='saveerror' class='error'>Failed to save settings.</div>
<div id='savesuccess' class='success'>Settings saved</div>

<div id="info" class="info">
<div class="stat"><span class="x t">Voltage:</span><span class="x v" id="voltage"></span></div>
<div class="stat"><span class="x t">Current:</span><span class="x v" id="current"></span></div>
<div class="stat"><span class="x t">Packet Errors:</span><span class="x v" id="badpkt"></span></div>
<div class="stat"><span class="x t">CRC Errors:</span><span class="x v" id="badcrc"></span></div>
<div class="stat"><span class="x t">Ignored request errors:</span><span class="x v" id="ignored"></span></div>
</div>

<div class="page" id="homePage">
<div class="graphs" style=""><div id="graph1" style="width:100%%; height:100%%;"></div></div>
</div>

<div class="page" id="aboutPage">
<h1>About</h1>

<h2>Source Code</h2>
<a href="https://github.com/stuartpittaway/diyBMSv4" target="_blank">https://github.com/stuartpittaway/diyBMSv4</a>

<h2>WARNING</h2>
<p>This is a DIY product/solution so don’t use this for safety critical systems or in any situation where there could be a risk to life.</p>
<p>There is no warranty, it may not work as expected or at all.</p>
<p>The use of this project is done so entirely at your own risk. It may involve electrical voltages which could kill - if in doubt, seek help.</p>
<p>The use of this project may not be compliant with local laws or regulations - if in doubt, seek help.</p>

<h2>License</h2>
<p>This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 2.0 UK: England & Wales License.</p>
<a href="https://creativecommons.org/licenses/by-nc-sa/2.0/uk/" target="_blank">https://creativecommons.org/licenses/by-nc-sa/2.0/uk/</a>

</div>

<div class="page" id="settingsPage">
    <h1>Settings</h1>
    <table id="settingsTable">
        <thead>
            <tr>
                <th>Bank</th>
                <th>Cell</th>
                <th>Voltage</th>
                <th>V. Min</th>
                <th>V. Max</th>
                <th>Temp Int °C</th>
                <th>Temp Ext °C</th>
            </tr>
        </thead>
        <tbody>
        </tbody>
    </table>

    <div id="settingConfig">
        <h2>Settings for </h2>
        <div id='waitforsettings'>Configuration data has been requested from cell module. Please wait 5 seconds and click button again.</div>
        <form id="settingsForm" method="POST" action="savesetting.json">
            <div class="settings">
                <input name="b" id="b" type="hidden" value="0">
                <input name="m" id="m" type="hidden" value="0">
                <div>
                    <label for="BypassOverTempShutdown">Bypass Over Temp Shutdown</label>
                    <input type="number" min="20" max="90" step="1" name="BypassOverTempShutdown" id="BypassOverTempShutdown" value="70" required="">
                </div>
                <div>
                    <label for="BypassThresholdmV">Bypass Threshold mV</label>
                    <input type="number" min="2500" max="4500" step="10" name="BypassThresholdmV" id="BypassThresholdmV" value="4100" required="">
                </div>
                <div>
                    <label for="Calib">Calibration multiplier</label>
                    <input id="Calib" name="Calib" type="number" min="0" max="5" step="0.0001" value="2.275862" required="">
                    <label for="ActualVoltage">Calculator - Actual measured voltage</label>
                    <input id="ActualVoltage" name="ActualVoltage" type="number" min="0" max="5" step="0.001" value="4.2">
                    <button type="button" id="CalculateCalibration">Calculate</button>
                </div>
                <div>
                    <label for="ExtBCoef">External temperature BCoef</label>
                    <input type="number" min="0" max="9999" step="1" id="ExtBCoef" name="ExtBCoef" value="4150" required="">
                </div>
                <div>
                    <label for="IntBCoef">Internal temperature BCoef</label>
                    <input type="number" min="0" max="9999" step="1" id="IntBCoef" name="IntBCoef" value="4150" required="">
                </div>
                <div>
                    <label for="LoadRes">Load resistance</label>
                    <input id="LoadRes" name="LoadRes" type="number" min="0" max="1000" step="0.01" value="4.4" required="">
                </div>
                <div>
                    <label for="mVPerADC">mV per ADC reading</label>
                    <input id="mVPerADC" name="mVPerADC" type="number" step="0.01" min="1" max="10" value="2" required="">
                </div>
                <input type="submit" value="Save settings"></input>
              </div>
        </form>
    </div>

    <div id="globalConfig">
    <h2>Global Settings</h2>
    <p>Configure all modules using these parameters</p>
    <form id="globalSettingsForm" method="POST" action="saveglobalsetting.json">
        <div class="settings">
            <div>
                <label for="g1">Bypass Over Temp Shutdown</label>
                <input type="number" min="20" max="90" step="1" name="BypassOverTempShutdown" id="g1" value="70" required="">
            </div>
            <div>
                <label for="g2">Bypass Threshold mV</label>
                <input type="number" min="2500" max="4500" step="10" name="BypassThresholdmV" id="g2" value="4100" required="">
            </div>
          </div>
          <input id="globalSettingsButton" type="submit" value="Save settings"></input>
        </div>
      </form>
</div>

<div class="page" id="integrationPage">
<h1>Integration</h1>


<h2>MQTT</h2>
<p>For security, you will need to re-enter the password for the services you want to enable or modify, before you Save</p>
<form id="mqttForm" method="POST" action="savemqtt.json">
    <div class="settings">
    <div>
        <label for="mqttEnabled">Enabled</label>
        <input type="checkbox" name="mqttEnabled" id="mqttEnabled" value="" required="">
    </div>
        <div>
            <label for="mqttServer">Server</label>
            <input type="input" name="mqttServer" id="mqttServer" value="" required=""  maxlength="64">
        </div>
        <div>
            <label for="mqttPort">Port</label>
            <input type="number" min="1" max="65535" step="1" name="mqttPort" id="mqttPort" value="1883" required="">
        </div>
        <div>
            <label for="mqttUsername">Username</label>
            <input type="input" name="mqttUsername" id="mqttUsername" value="" required="" maxlength="32">
        </div>
        <div>
            <label for="mqttPassword">Password</label>
            <input type="password" name="mqttPassword" id="mqttPassword" value="" required="" maxlength="32">
        </div>
        <input type="submit" value="Save MQTT settings"></input>
    </div>
</form>
</div>


<h2>Influx Database</h2>
<form id="influxForm" method="POST" action="saveinfluxdb.json">
    <div class="settings">
    <div>
        <label for="influxEnabled">Enabled</label>
        <input type="checkbox" name="influxEnabled" id="influxEnabled" value="" required="">
    </div>
        <div>
            <label for="influxServer">Host server</label>
            <input type="input" name="influxServer" id="influxServer" value="influx" required=""  maxlength="64">
        </div>
        <div>
            <label for="influxPort">Port</label>
            <input type="number" min="1" max="65535" step="1" name="influxPort" id="influxPort" value="1883" required="">
        </div>
        <div>
            <label for="influxDatabase">Database name</label>
            <input type="input" name="influxDatabase" id="influxDatabase" value="database" required=""  maxlength="64">
        </div>
        <div>
            <label for="influxUsername">Username</label>
            <input type="input" name="influxUsername" id="influxUsername" value="myusername" required="" maxlength="32">
        </div>
        <div>
            <label for="influxPassword">Password</label>
            <input type="password" name="influxPassword" id="influxPassword" value="" required="" maxlength="32">
        </div>
        <input type="submit" value="Save Influx DB settings"></input>
    </div>
</form>
</div>


</div>

<script type="text/javascript">
var g1=null;
var g2=null;
var g3=null;
var XSS_KEY='%XSS_KEY%';

function identifyModule(button, bank, module) {
  //Populate settings div
  $.getJSON( "identifyModule.json",
  {       b: bank,       m: module    },
    function(data) {    }
  ).fail(function() {   $("#iperror").show();});
}

function configureModule(button, bank, module) {

  //Select correct row in table
  $(button).parent().parent().parent().find(".selected").removeClass("selected");
  $(button).parent().parent().addClass("selected");

  //Populate settings div
  $("#settingConfig h2").html("Settings for bank:"+bank+" module:"+module);
  $("#settingConfig").show();

  $.getJSON( "settings.json",
  {
       b: bank,
       m: module
    },
    function(data) {

      var div=$("#settingConfig .settings");
      $('#b').val( data.settings.bank);
      $('#m').val(data.settings.module);

      if (data.settings.Cached==true){
        $('#BypassOverTempShutdown').val(data.settings.BypassOverTempShutdown);
        $('#BypassThresholdmV').val(data.settings.BypassThresholdmV);
        $('#Calib').val(data.settings.Calib.toFixed(4));
        $('#ExtBCoef').val(data.settings.ExtBCoef);
        $('#IntBCoef').val(data.settings.IntBCoef);
        $('#LoadRes').val(data.settings.LoadRes.toFixed(2));
        $('#mVPerADC').val(data.settings.mVPerADC.toFixed(2));
        $('#settingsForm').show();
        $('#waitforsettings').hide();
      } else {
        //Data not ready yet
        $('#settingsForm').hide();
        $('#waitforsettings').show();
        //Call back in 5 seconds to refresh page - this is a bad idea!
        //setTimeout(configureModule, 5000, button, bank, module);
      }
    }).fail(function() {
     $("#iperror").show();
  });
}

function queryBMS() {
  $.getJSON( "monitor.json", function( jsondata ) {

    var voltage=0.0;
    var labels=[];
    var bank=[];
    var voltages=[];
    var voltagesmin=[];
    var voltagesmax=[];
    var tempint=[];
    var tempext=[];

    for (var bankNumber = 0; bankNumber <4; bankNumber++) {
    //Need to cater for banks of cells
    $.each(jsondata.bank[bankNumber], function( index, value ) {
      var color=value.bypass ? "#B03A5B":"#1e90ff";
      voltages.push( { value: (parseFloat(value.v)/1000.0), itemStyle:{color: color } } );

      voltagesmin.push((parseFloat(value.minv)/1000.0));
      voltagesmax.push((parseFloat(value.maxv)/1000.0));

      bank.push(bankNumber);
      labels.push(index);

      color=value.bypasshot ? "#B03A5B":"#1e90ff";
      tempint.push({ value: value.int, itemStyle:{color: color } });

      tempext.push(value.ext==-40 ? 0:value.ext  );

      //TODO: This needs to be voltage per bank not total
      voltage+=(parseFloat(value.v)/1000.0);
    });
  }


    $("#badcrc").html(jsondata.monitor.badcrc);
    $("#badpkt").html(jsondata.monitor.badpkt);
    $("#ignored").html(jsondata.monitor.ignored);

    $("#voltage").html(voltage.toFixed(2));

    $("#current").html("");

    if (jsondata.monitor.commserr==true) {
      $("#commserr").show();
    } else {
      $("#commserr").fadeOut();
    }

    $("#iperror").hide();


    if($('#settingsPage').is(':visible')){
        var tbody=$("#settingsTable tbody");

        if ($('#settingsTable tbody tr').length!=labels.length) {
            $("#settingConfig").hide();
            //Add rows if they dont exist (or incorrect amount)
            $(tbody).find("tr").remove();

            $.each(labels, function( index, value ) {
                $(tbody).append("<tr><td>"+bank[index]+"</td><td>"+value+"</td><td></td><td></td><td></td><td></td><td></td><td><button type='button' onclick='return identifyModule(this,"+bank[index]+","+value+");'>Identify</button></td><td><button type='button' onclick='return configureModule(this,"+bank[index]+","+value+");'>Configure</button></td></tr>")
            });
        }

        var rows=$(tbody).find("tr");

        $.each(labels, function( index, value ) {
            var columns=$(rows[index]).find("td");

            //$(columns[0]).html(value);
            $(columns[2]).html(voltages[index].value.toFixed(3));
            $(columns[3]).html(voltagesmin[index].toFixed(3));
            $(columns[4]).html(voltagesmax[index].toFixed(3));
            $(columns[5]).html(tempint[index].value);
            $(columns[6]).html(tempext[index]);
        });
    }


  if($('#homePage').is(':visible')){

      if (g1==null) {
        // based on prepared DOM, initialize echarts instance
        g1 = echarts.init(document.getElementById('graph1'));

        var labelOption = {
            normal: {
                show: true,
                position: 'insideBottom',
                distance: 15,
                align: 'left',
                verticalAlign: 'middle',
                rotate: 90,
                formatter: '{c}V',
                fontSize: 24
            }
        };

        var labelOption3 = {
            normal: {
                show: true,
                position: 'top',
                distance:5,
                formatter: '{c}V',
                fontSize: 14
            }
        };

        var labelOption4 = {
            normal: {
                show: true,
                position: 'bottom',
                distance:5,
                formatter: '{c}V',
                fontSize: 14
            }
        };


            var labelOption2 = {
                  normal: {
                      show: true,
                      position: 'insideBottom',
                      distance: 15,
                      align: 'left',
                      verticalAlign: 'middle',
                      rotate: 90,
                      formatter: '{c}°C',
                      fontSize: 22
                  }
              };
        // specify chart configuration item and data
        var option = {
          color: ['#003366', '#006699', '#4cabce'],
          tooltip: { trigger: 'axis', axisPointer: { type: 'cross', crossStyle: { color: '#999' } } },
            legend: { data:['Voltage'] },
            xAxis: [{gridIndex: 0,type:'category' },{  gridIndex: 1,type:'category' }],
            yAxis: [
              {gridIndex: 0,name:'Volts',type:'value',min:2.5,max:5.0,interval:0.25,position:'left', axisLabel: { formatter: '{value}V' }}
              ,{gridIndex: 1,name:'Temperature',type:'value',interval:10,position:'right'
              ,axisLabel:{ formatter: '{value} °C' }
              ,axisLine:{show:false, lineStyle:{type:'dotted'} } } ]
            ,series: [{ name: 'Voltage', type: 'bar', data: [], label: labelOption
           }
                ,{ name: 'Min V', type: 'line', data: [], label: labelOption4,symbolSize: 20, symbol:['circle'], itemStyle:{normal:{lineStyle:{color:'transparent',type:'dotted'}} } }
                ,{ name: 'Max V', type: 'line', data: [], label: labelOption3,symbolSize: 20, symbol:['triangle'], itemStyle:{normal:{lineStyle:{color:'transparent',type:'dotted'}} } }
                ,{xAxisIndex:1, yAxisIndex:1, name:'BypassTemperature',type:'bar', data: [], label: labelOption2 }
                ,{xAxisIndex:1, yAxisIndex:1, name:'CellTemperature',type:'bar',data: [], label: labelOption2 }
            ],
            grid: [{containLabel:false,
              left:'5%%',
              right:'5%%',
              bottom:'32%%'},{containLabel:false,
                left:'5%%',
                right:'5%%',
                top:'78%%'}],
        };

        // use configuration item and data specified to show chart
        g1.setOption(option);

      } else {
        g1.setOption({
            xAxis: { data: labels },
            series: [{ name: 'Voltage', data: voltages }
            ,{ name: 'Min V', data: voltagesmin }
            ,{ name: 'Max V', data: voltagesmax }
            ,{ name: 'BypassTemperature', data: tempint }
            ,{ name: 'CellTemperature', data: tempext }]
        });
      }
  }

  }).fail(function() {
     $("#iperror").show();
  });
}

function countdown() {
  queryBMS();
  $("#refreshbar").width('100%%').animate({ width: '-=100%%' },{ duration:4000, complete: countdown, queue:false});
}

$(function() {
  //On page ready
  countdown();

  $('#CalculateCalibration').click(function() {
    var currentReading=parseFloat($("#settingsTable > tbody > tr.selected > td:nth-child(3)").text());
    var currentCalib=parseFloat($("#Calib").val());
    var actualV=parseFloat($("#ActualVoltage").val());
    var result=(currentCalib/currentReading)*actualV;
    $("#Calib").val(result.toFixed(4));
    return true;
  });

  $("#home").click(function() {
    $(".header-right a").removeClass("active");
    $(this).addClass("active");
    $(".page").hide();
    $("#homePage").show();
    return true;
  });

  $("#about").click(function() {
    $(".header-right a").removeClass("active");
    $(this).addClass("active");
    $(".page").hide();
    $("#aboutPage").show();
    return true;
  });

  $("#settings").click(function() {
    $(".header-right a").removeClass("active");
    $(this).addClass("active");
    $(".page").hide();


    //Remove existing table
    $("#settingsTable tbody").find("tr").remove();
    $("#settingConfig").hide();
    $("#settingsPage").show();
    return true;
  });

  $("#integration").click(function() {
    $(".header-right a").removeClass("active");
    $(this).addClass("active");
    $(".page").hide();
    $("#integrationPage").show();

    $("#mqttForm").hide();
    $("#influxForm").hide();

    $.getJSON( "integration.json",
      function(data) {

          $('#mqttEnabled').val(data.mqtt.enabled);
          $('#mqttServer').val(data.mqtt.server);
          $('#mqttPort').val(data.mqtt.port);
          $('#mqttUsername').val(data.mqtt.username);
          $('#mqttPassword').val("");

          $('#influxEnabled').val(data.influxdb.enabled);
          $('#influxServer').val(data.influxdb.server);
          $('#influxPort').val(data.influxdb.port);
          $('#influxDatabase').val(data.influxdb.database);
          $('#influxUsername').val(data.influxdb.username);
          $('#influxPassword').val("");

          $("#mqttForm").show();
          $("#influxForm").show();
      }).fail(function() {}
    );

    return true;
  });

  $("#settingsForm").submit(function (e) {
       e.preventDefault();

       $.ajax({
           type: $(this).attr('method'),
           url: $(this).attr('action'),
           data: $(this).serialize(),
           success: function (data) {
               $('#settingConfig').hide();
               $("#savesuccess").show().delay(2000).fadeOut(500);
           },
           error: function (data) {
               $("#saveerror").show().delay(2000).fadeOut(500);
           },
       });
   });

   $("#globalSettingsForm").submit(function (e) {
        e.preventDefault();

        $.ajax({
            type: $(this).attr('method'),
            url: $(this).attr('action'),
            data: $(this).serialize(),
            success: function (data) {
              $("#savesuccess").show().delay(2000).fadeOut(500);
            },
            error: function (data) {
                $("#saveerror").show().delay(2000).fadeOut(500);
            },
        });
    });


  $("#homePage").show();
});
</script>
</body></html>
)=====";
