const char FILE_INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width=device-width,initial-scale=1.0">
<link href="https://fonts.googleapis.com/css?family=Nova+Mono" rel="stylesheet">
<link href="style.css" rel="stylesheet">
<title>DIY BMS CONTROLLER v4</title>
<script src="jquery.js" integrity="sha384-tsQFqpEReu7ZLhBV2VZlAu7zcOV+rXbYlF2cqB8txI/8aZajjp4Bqd+V6D5IgvKT" crossorigin="anonymous"></script>
<script src="echarts.simple.min.js" integrity="" crossorigin="anonymous"></script>
</head>
<body>
<div class="header">
  <div class="logocontainer">
  <img class="logo" src="logo.gif" width="191" height="48" alt="DIYBMS"/>
  <div id="refreshbar"></div>
  </div>
  <div class="header-right">
    <a id="home" class="active" href="#home">Home</a>
    <a id="modules" href="#modules">Modules</a>
    <a id="integration" href="#integration">Integration</a>
    <a id="settings" href="#settings">Settings</a>
    <a id="about" href="#about">About</a>
  </div>
</div>

<div id='commserr' class='error'>The controller is having difficulty communicating with the cell monitoring modules.</div>
<div id='iperror' class='error'>Cannot communicate with the controller for status updates.</div>
<div id='saveerror' class='error'>Failed to save settings.</div>
<div id='savesuccess' class='success'>Settings saved</div>

<div id="info" class="info">
<div id="voltage1" class="stat"><span class="x t">Voltage 0:</span><span class="x v"></span></div>
<div id="voltage2" class="stat"><span class="x t">Voltage 1:</span><span class="x v"></span></div>
<div id="voltage3" class="stat"><span class="x t">Voltage 2:</span><span class="x v"></span></div>
<div id="voltage4" class="stat"><span class="x t">Voltage 3:</span><span class="x v"></span></div>

<div id="range1" class="stat"><span class="x t">Range 0:</span><span class="x v"></span></div>
<div id="range2" class="stat"><span class="x t">Range 1:</span><span class="x v"></span></div>
<div id="range3" class="stat"><span class="x t">Range 2:</span><span class="x v"></span></div>
<div id="range4" class="stat"><span class="x t">Range 3:</span><span class="x v"></span></div>


<div id="current" class="stat"><span class="x t">Current:</span><span class="x v"></span></div>
<div id="badpkt" class="stat"><span class="x t">Packet Errors:</span><span class="x v"></span></div>
<div id="badcrc" class="stat"><span class="x t">CRC Errors:</span><span class="x v"></span></div>
<div id="ignored" class="stat"><span class="x t">Ignored request errors:</span><span class="x v"></span></div>
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
    <h1>Controller Settings</h1>
    <h2>Banks</h2>
    <p>DIYBMS supports up to 16 modules in a single bank. Up to 4 seperate banks can be configured.  Only enable a bank if you need this advanced functionality as it slows down processing and can cause errors.</p>
    <p>Combination type: Use Parallel when you have multiple banks up to 16S, or Serial if you want a single bank with up to 64S.</p>
    <form id="banksForm" method="POST" action="savebankconfig.json" autocomplete="off">
      <div class="settings">
        <div>
            <label for="totalBanks">Total number of banks</label>
            <select name="totalBanks" id="totalBanks"><option>1</option><option>2</option><option>3</option><option>4</option></select>
        </div>
        <div>
            <label for="combitype">Bank combination type</label>
            <select name="combitype" id="combitype"><option>Parallel</option><option>Serial</option></select>
        </div>
        <input type="submit" value="Save bank settings"></input>
      </div>
    </form>
</div>

<div class="page" id="modulesPage">
    <h1>Modules</h1>

    <div id="modulesTable">
    <div class="th">
    <span>Bank</span>
    <span>Cell</span>
    <span>Voltage</span>
    <span class='hide'>V. Min</span>
    <span class='hide'>V. Max</span>
    <span class='hide'>Temp<br/>Int °C</span>
    <span class='hide'>Temp<br/>Ext °C</span>
    </div>
        <div class="rows" id="modulesRows"></div>
    </div>

    <div id="settingConfig">
        <h2>Settings for module </h2>
        <div id='waitforsettings'>Configuration data has been requested from cell module. Please wait 5 seconds and click button again.</div>
        <form id="settingsForm" method="POST" action="savesetting.json" autocomplete="off">
            <div class="settings">
                <input name="b" id="b" type="hidden" value="0">
                <input name="m" id="m" type="hidden" value="0">
                <div>
                    <label for="BypassOverTempShutdown">Bypass over temperature</label>
                    <input type="number" min="20" max="90" step="1" name="BypassOverTempShutdown" id="BypassOverTempShutdown" value="70" required="">
                </div>
                <div>
                    <label for="BypassThresholdmV">Bypass threshold mV</label>
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
                <div>
                    <label for="movetobank">Move to bank</label>
                    <select id="movetobank" name="movetobank"><option>0</option><option>1</option><option>2</option><option>3</option></select>
                </div>
                <input type="submit" value="Save settings"></input>
              </div>
        </form>
    </div>

    <div id="globalConfig">
    <h2>Global Settings</h2>
    <p>Configure all connected modules (in selected bank) to use following parameters:</p>
    <form id="globalSettingsForm" method="POST" action="saveglobalsetting.json" autocomplete="off">
        <div class="settings">
            <div>
                <label for="g1">Bypass over temperature</label>
                <input type="number" min="20" max="90" step="1" name="BypassOverTempShutdown" id="g1" value="70" required="">
            </div>
            <div>
                <label for="g2">Bypass threshold mV</label>
                <input type="number" min="2500" max="4500" step="10" name="BypassThresholdmV" id="g2" value="4100" required="">
            </div>
          </div>
          <input id="globalSettingsButton" type="submit" value="Save settings"></input>
        </div>
      </form>
</div>

<div class="page" id="integrationPage">
<h1>Integration</h1>
<p>For security, you will need to re-enter the password for the service(s) you want to enable or modify, before you save.</p>
<p>After changes are made, the controller will automatically reboot.  You will need to refresh the web page to continue.</p>
<h2>MQTT</h2>
<form id="mqttForm" method="POST" action="savemqtt.json" autocomplete="off">
    <div class="settings">
    <div>
        <label for="mqttEnabled">Enabled</label>
        <input type="checkbox" name="mqttEnabled" id="mqttEnabled">
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


<h2>Influx Database</h2>
<form id="influxForm" method="POST" action="saveinfluxdb.json"  autocomplete="off">
    <div class="settings">
    <div>
        <label for="influxEnabled">Enabled</label>
        <input type="checkbox" name="influxEnabled" id="influxEnabled">
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
  $("#settingConfig h2").html("Settings for module bank:"+bank+" module:"+module);
  $("#settingConfig").show();

  $.getJSON( "modules.json",
  {
       b: bank,
       m: module
    },
    function(data) {

      var div=$("#settingConfig .settings");
      $('#b').val(data.settings.bank);
      $('#m').val(data.settings.module);

      if (data.settings.Cached==true){
        $('#BypassOverTempShutdown').val(data.settings.BypassOverTempShutdown);
        $('#BypassThresholdmV').val(data.settings.BypassThresholdmV);
        $('#Calib').val(data.settings.Calib.toFixed(4));
        $('#ExtBCoef').val(data.settings.ExtBCoef);
        $('#IntBCoef').val(data.settings.IntBCoef);
        $('#LoadRes').val(data.settings.LoadRes.toFixed(2));
        $('#mVPerADC').val(data.settings.mVPerADC.toFixed(2));
        $('#movetobank').val(data.settings.bank);

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
    var labels = [];
    var cells = [];
    var bank = [];
    var voltages = [];
    var voltagesmin = [];
    var voltagesmax = [];
    var tempint = [];
    var tempext = [];

    var voltage = [0.0, 0.0, 0.0, 0.0];
    //Not currently supported
    var current = [0.0, 0.0, 0.0, 0.0];

    var bankmin = [5000,5000,5000,5000];
    var bankmax = [0.0,0.0,0.0,0.0];

    for (var bankNumber = 0; bankNumber < jsondata.banks; bankNumber++) {
        //Need to cater for banks of cells
        $.each(jsondata.bank[bankNumber], function(index, value) {
            var color = value.bypass ? "#B03A5B" : "#1e90ff";

            var v = (parseFloat(value.v) / 1000.0);

            voltages.push({ value: v, itemStyle: { color: color } });

            voltagesmin.push((parseFloat(value.minv) / 1000.0));
            voltagesmax.push((parseFloat(value.maxv) / 1000.0));

            //TODO: This looks incorrect needs to take into account bank/cell configs
            bank.push(bankNumber);
            cells.push(index);


            labels.push(bankNumber + "/" + index);

            color = value.bypasshot ? "#B03A5B" : "#1e90ff";
            tempint.push({ value: value.int, itemStyle: { color: color } });

            tempext.push(value.ext == -40 ? 0 : value.ext);

            var bIndex=jsondata.parallel ? bankNumber:0;
            voltage[bIndex] += v;
            if (value.v<bankmin[bIndex]) {bankmin[bIndex]=value.v;}
            if (value.v>bankmax[bIndex]) {bankmax[bIndex]=value.v;}
        });
    }

    //Ignore and hide any errors which are zero
    if (jsondata.monitor.badcrc==0) { $("#badcrc").hide(); } else { $("#badcrc .v").html(jsondata.monitor.badcrc);$("#badcrc").show();}
    if (jsondata.monitor.badpkt==0) { $("#badpkt").hide(); } else { $("#badpkt .v").html(jsondata.monitor.badpkt);$("#badpkt").show();}
    if (jsondata.monitor.ignored==0) { $("#ignored").hide(); } else { $("#ignored .v").html(jsondata.monitor.ignored);$("#ignored").show();}

    for (var bankNumber = 0; bankNumber < 4; bankNumber++) {
      if (voltage[bankNumber]>0) {
        $("#voltage"+(bankNumber+1)+" .v").html(voltage[bankNumber].toFixed(2)+"V");
        var range=bankmax[bankNumber]-bankmin[bankNumber];
        $("#range"+(bankNumber+1)+" .v").html(range+"mV");

        $("#voltage"+(bankNumber+1)).show();
        $("#range"+(bankNumber+1)).show();
      } else {
        $("#voltage"+(bankNumber+1)).hide();
        $("#range"+(bankNumber+1)).hide();
      }

    }

    //Not currently supported
    $("#current").hide();
    $("#current .v").html(current[0].toFixed(2));

    if (jsondata.monitor.commserr==true) {
      $("#commserr").show();
    } else {
      $("#commserr").fadeOut();
    }

    $("#iperror").hide();

    if($('#modulesPage').is(':visible')){
        var tbody=$("#modulesRows");

        if ($('#modulesRows div').length!=cells.length) {
            $("#settingConfig").hide();

            //Add rows if they dont exist (or incorrect amount)
            $(tbody).find("div").remove();

            $.each(cells, function( index, value ) {
                $(tbody).append("<div><span>"
                +bank[index]
                +"</span><span>"
                +value
                +"</span><span></span><span class='hide'></span><span class='hide'></span><span class='hide'></span><span class='hide'></span><span><button type='button' onclick='return identifyModule(this,"
                +bank[index]+","+value+");'>Identify</button></span><span><button type='button' onclick='return configureModule(this,"
                +bank[index]+","+value+");'>Configure</button></span></div>")
            });
        }

        var rows=$(tbody).find("div");

        $.each(cells, function( index, value ) {
            var columns=$(rows[index]).find("span");

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
              {gridIndex: 0,name:'Volts',type:'value',min:2.5,max:4.5,interval:0.25,position:'left', axisLabel: { formatter: '{value}V' }}
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
    var currentReading=parseFloat($("#modulesRows > div.selected > span:nth-child(3)").text());
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

  $("#modules").click(function() {
    $(".header-right a").removeClass("active");
    $(this).addClass("active");
    $(".page").hide();

    //Remove existing table
    $("#modulesRows").find("div").remove();

    $("#settingConfig").hide();
    $("#modulesPage").show();
    return true;
  });

  $("#settings").click(function() {
    $(".header-right a").removeClass("active");
    $(this).addClass("active");
    $(".page").hide();

    $("#banksForm").hide();
    $("#settingsPage").show();

    $.getJSON( "settings.json",
      function(data) {
          $("#totalBanks").val(data.settings.totalnumberofbanks);
          if (data.settings.combinationparallel) {
          $("#combitype").val("Parallel");
        } else {
          $("#combitype").val("Serial");
        }

          $("#banksForm").show();
      }).fail(function() {}
    );

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

          $("#mqttEnabled").prop("checked", data.mqtt.enabled );
          $("#mqttServer").val(data.mqtt.server);
          $("#mqttPort").val(data.mqtt.port);
          $("#mqttUsername").val(data.mqtt.username);
          $("#mqttPassword").val("");

          $("#influxEnabled").prop("checked", data.influxdb.enabled );
          $("#influxServer").val(data.influxdb.server);
          $("#influxPort").val(data.influxdb.port);
          $("#influxDatabase").val(data.influxdb.database);
          $("#influxUsername").val(data.influxdb.username);
          $("#influxPassword").val("");

          $("#mqttForm").show();
          $("#influxForm").show();
      }).fail(function() {}
    );

    return true;
  });


$("#mqttForm").submit(function (e) {
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

 $("#influxForm").submit(function (e) {
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


  $("#settingsForm").submit(function (e) {
       e.preventDefault();

       $.ajax({
           type: $(this).attr('method'),
           url: $(this).attr('action'),
           data: $(this).serialize(),
           success: function (data) {
               $('#settingConfig').hide();
               $("#savesuccess").show().delay(2000).fadeOut(500);

               if ($("#b").val() !== $("#movetobank").val()) {
                 //Remove existing table as we have moved banks
                 $("#modulesRows").find("div").remove();
               }
           },
           error: function (data) {
               $("#saveerror").show().delay(2000).fadeOut(500);
           },
       });
   });



  $("#banksForm").submit(function (e) {
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

    $("#mqttEnabled").change(function() {
        if($(this).is(":checked")) {
          $("#mqttForm").removeAttr("novalidate");
        } else {
          $("#mqttForm").attr("novalidate","");
        }
    });

    $("#influxEnabled").change(function() {
        if($(this).is(":checked")) {
          $("#influxForm").removeAttr("novalidate");
        } else {
          $("#influxForm").attr("novalidate","");
        }
    });

    $.ajaxSetup({
        beforeSend:function (xhr, settings){settings.data += '&xss='+XSS_KEY;}
    });

  $("#homePage").show();
});
</script>
</body></html>
)=====";
