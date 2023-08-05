function toggleButtonSwitch(value) {
  value = (value ? 11 : 0);
  updateSlider(value)
}

function updateSlider(value) {
  document.getElementById("sliderLabel").innerHTML = value;
  document.getElementById("sliderId").value = value;
  send(value);
}

function send(value) {
  console.log('dimmer value:', value);
  fetch(`/toggle?status=${value}`).then(res => console.log(res));
}
