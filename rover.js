$(function () {
var ip = "http://192.168.1.1/";



$('#on').click(function(){
$.get(ip + 'led/on', function(data){
  console.log("turn light on");
});

});



$('#off').click(function(){
$.get(ip + 'led/off', function(data){
  console.log("turn light off");
});

});




$(document).on('input', '#myRange', function(){
  var speed = this.value * -1;
  var test = Math.abs(speed);
  if (test % 10 == 0){
    console.log(speed);
    $.get(ip + 'speed/' + speed).done(function(data){
      console.log("drive straight with " + speed);
    });
  }
});

$(document).on('mouseup', '#myRange', function(){
this.value = 0;
$.get(ip + 'speed/' + 0).done(function(data){
  console.log("drive straight with " + 0);
});

});

});
