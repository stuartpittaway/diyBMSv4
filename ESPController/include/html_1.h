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
.logo { float:left; background-image: url("logo.gif");  width:191px;  height:48px;}

.info {width:100%;}
.info .stat {display:block;width:10%;border:solid 2px gray;padding:2px;margin:2px;float:left;min-width:175px;}

.info .stat .x {display:block;width:100%;text-align:right;}
.info .stat .t {font-size:10pt;color:darkgray;}
.info .stat .v {font-size:22pt;font-family: 'Nova Mono', monospace;font-weight:bold;color:#1e90ff;}
#refreshbar { width:100%; padding:0; margin:0; height:4px; background-color:#d3f9fa;}
.graphs {height:768px;width:100%; }
.ct-series-a .ct-bar {
  stroke: purple;
  stroke-width: 25px;
}

@media screen and (max-width:500px){
  .header a,.header-right{float:none}
  .header a{display:block;text-align:left}
  .graphs {height:320px;}
}

@media screen and (max-width:640px){
  .graphs {height:500px;}
}

.error:before { content: '!'; }
.error { color:#D8000C;background-color:#FFBABA; padding:10px;display:none; width:100%;}

.page {clear:both; width:100%; display:none;}

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

<div id="info" class="info">
<div class="stat"><span class="x t">Voltage:</span><span class="x v" id="voltage"></span></div>
<div class="stat"><span class="x t">Current:</span><span class="x v" id="current"></span></div>
<div class="stat"><span class="x t">Packet Errors:</span><span class="x v" id="badpkt"></span></div>
<div class="stat"><span class="x t">CRC Errors:</span><span class="x v" id="badcrc"></span></div>
<div class="stat"><span class="x t">Ignored request errors:</span><span class="x v" id="ignored"></span></div>
</div>

<div class="page" id="homePage">
<div class="graphs" style=""><div id="graph1" style="width:100%; height:100%;"></div></div>
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
<form id="settingsForm" method="POST" action="savesetting">
<div class="settings"></div>
</div>
</form>
</div>

<div class="page" id="integrationPage">
<h1>Integration</h1>
</div>

<script type="text/javascript">
var g1=null;
var g2=null;
var g3=null;


function configureModule(button, bank, module) {
  console.log(button,bank,module);

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
      $(div).empty();
      $(div).append("<input id='bank' type='hidden' value='"+data.settings.bank+"'/>");
      $(div).append("<input id='module' type='hidden' value='"+data.settings.module+"'/>");

      if (data.settings.Cached==true && data.settings.Requested==false){

      $(div).append("<div><label for='BypassOverTempShutdown'>Bypass Over Temp Shutdown</label><input id='BypassOverTempShutdown' type='input' value='"+data.settings.BypassOverTempShutdown+"'/></div>");
      $(div).append("<div><label for='BypassThresholdmV'>Bypass Threshold mV</label><input id='BypassThresholdmV' type='input' value='"+data.settings.BypassThresholdmV+"'/></div>");
      $(div).append("<div><label for='Calib'>Calibration multiplier</label><input id='Calib' type='input' value='"+data.settings.Calib+"'/></div>");
      $(div).append("<div><label for='ExtBCoef'>External temperature BCoef</label><input id='ExtBCoef' type='input' value='"+data.settings.ExtBCoef+"'/></div>");
      $(div).append("<div><label for='IntBCoef'>Internal temperature BCoef</label><input id='IntBCoef' type='input' value='"+data.settings.IntBCoef+"'/></div>");
      $(div).append("<div><label for='LoadRes'>Load resistance</label><input id='LoadRes' type='input' value='"+data.settings.LoadRes+"'/></div>");
      $(div).append("<div><label for='mVPerADC'>mV per ADC reading</label><input id='mVPerADC' type='input' value='"+data.settings.mVPerADC+"'/></div>");
    } else {
      //Data not ready yet
      $(div).append("<div>Configuration data has been requested from cell module.  Please wait 5 seconds and click button again.</div>");

      //Call back in 5 seconds to refresh page - this is a bad idea!
      //setTimeout(configureModule, 5000, button, bank, module);
    }
/*
Cached: false
Requested: true
*/

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
      voltages.push((parseFloat(value.v)/1000.0));

      voltagesmin.push((parseFloat(value.minv)/1000.0));
      voltagesmax.push((parseFloat(value.maxv)/1000.0));

      bank.push(bankNumber);
      labels.push(index);

      tempint.push(value.int);
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
                $(tbody).append("<tr><td>"+bank[index]+"</td><td>"+value+"</td><td></td><td></td><td></td><td></td><td></td><td><button type='button' onclick='return configureModule(this,"+bank[index]+","+value+");'>Configure</button></td></tr>")
            });
        }

        var rows=$(tbody).find("tr");

        $.each(labels, function( index, value ) {
            var columns=$(rows[index]).find("td");

            //$(columns[0]).html(value);
            $(columns[2]).html(voltages[index].toFixed(3));
            $(columns[3]).html(voltagesmin[index].toFixed(3));
            $(columns[4]).html(voltagesmax[index].toFixed(3));
            $(columns[5]).html(tempint[index]);
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
                fontSize: 20
            }
        };

        var labelOption1 = {
            normal: {
                show: true,
                position: 'end',
                formatter: '{c}V',
                fontSize: 20
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
                      fontSize: 20
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
            ,series: [{ name: 'Voltage', type: 'bar', data: [], label: labelOption }
                ,{ name: 'Min V', type: 'line', data: [], label: labelOption1 }
                ,{ name: 'Max V', type: 'line', data: [], label: labelOption1 }
                ,{xAxisIndex:1, yAxisIndex:1, name:'BypassTemperature',type:'bar', data: [], label: labelOption2 }
                ,{xAxisIndex:1, yAxisIndex:1, name:'CellTemperature',type:'bar',data: [], label: labelOption2 }
            ],
            grid: [{containLabel:false,left:'5%',right:'5%',bottom:'32%'},{containLabel:false, left:'5%',right:'5%',top:'78%'}],
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
  $("#refreshbar").width('100%').animate({ width: '-=100%' },{ duration:4000, complete: countdown, queue:false});
}

$(function() {
  //On page ready
  countdown();

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
    return true;
  });

  $("#homePage").show();
});
</script>
</body>
</html>
)=====";
