function switchOn(value) {
  if (value) {
    updateSlider(100);
    document.getElementById("switch_off").checked = false;
  }
}

function switchOff(value) {
  if (value) {
    updateSlider(0);
    document.getElementById("switch_on").checked = false;
  }
}

function updateSlider(value) {
  document.getElementById("sliderLabel").innerHTML = value;
  document.getElementById("sliderId").value = value;
  send(value);
}

function send(value) {
  console.log('dimmer value:', value);
  fetch(`/dimmer?power=${value}`).then(res => console.log(res));
}
