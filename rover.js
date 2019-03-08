$(function () {
var ip = "http://192.168.1.1/";
var prevSpeed = 0;
var prevTurn = 0;



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




$(document).on('input', '#myRangeVert', function(){
  var speed = this.value * -1;
  console.log(speed);
  if (prevSpeed != speed){
  var test = Math.abs(speed);
  if (test % 10 == 0){
    console.log(speed);
    $.get(ip + 'speed/' + speed).done(function(data){
      console.log("drive straight with " + speed);
      prevSpeed = speed;
    });
  }
}
});

$('#stop').click(function(){
  $.get(ip + 'speed/' + 0).done(function(data){
    console.log("drive straight with " + 0);
  });
  });


$(document).on('input', '#myRangeHoriz', function(){
  var turn = this.value;
  console.log (turn);
  if (prevTurn != turn){
  var test = Math.abs(turn);
  if (test % 5 == 0){
    console.log(turn);
    $.get(ip + 'turn/' + turn).done(function(data){
      console.log("turn " + turn);
      prevTurn = turn;
    });
  }
}
});

$(document).on('mouseup', '#myRangeHoriz', function(){
this.value = 0;
$.get(ip + 'turn/' + 0).done(function(data){
  console.log("turn " + 0);
});

});

// $(document).on('mouseup', '#myRangeVert', function(){
// this.value = 0;
// $.get(ip + 'speed/' + 0).done(function(data){
//   console.log("drive with " + 0);
// });
//
// });

// $(document).on('mouseup', '#myRangeVert', function(){
// this.value = 0;
// $.get(ip + 'speed/' + 0).done(function(data){
//   console.log("drive straight with " + 0);
// });
//
// });

});
