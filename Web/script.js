
$(function(){

$("#temperature1_btn").click(function() {
	//this.classList.add("active");	
   $.get("http://192.168.100.4:8082/socket?temperature=1", function(data){
            if (data.success) {
                $("#result").html(data.success.msg);
            } else {
                $("#result").html(data.error.msg);
            }
        }, "json");
    return false;
});
$("#temperature0_btn").click(function() {
    $.get("http://192.168.100.4:8082/socket?temperature=0", function(data){
            if (data.success) {
                $("#result").html(data.success.msg);
            } else {
                $("#result").html(data.error.msg);
            }
        }, "json");
    return false;
});
$("#humidity1_btn").click(function() {
   $.get("http://192.168.100.4:8082/socket?humidity=1", function(data){
            if (data.success) {
                $("#result").html(data.success.msg);
            } else {
                $("#result").html(data.error.msg);
            }
        }, "json");
    return false;
});
$("#humidity0_btn").click(function() {
   $.get("http://192.168.100.4:8082/socket?humidity=0", function(data){
            if (data.success) {
                $("#result").html(data.success.msg);
            } else {
                $("#result").html(data.error.msg);
            }
        }, "json");
    return false;
});
$("#ventilation1_btn").click(function() {
   $.get("http://192.168.100.4:8082/socket?ventilation=1", function(data){
            if (data.success) {
                $("#result").html(data.success.msg);
            } else {
                $("#result").html(data.error.msg);
            }
        }, "json");
    return false;
});
$("#ventilation0_btn").click(function() {
   $.get("http://192.168.100.4:8082/socket?ventilation=0", function(data){
            if (data.success) {
                $("#result").html(data.success.msg);
            } else {
                $("#result").html(data.error.msg);
            }
        }, "json");
    return false;
});
});
