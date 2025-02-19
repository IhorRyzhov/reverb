clc; % Clears the command window

h = [2,1,2,3];
x = [7,4];
res = conv(h, x);
disp(res);

h1f = fft(h(1:2),4);
h2f = fft(h(3:4),4);
prev_f = [0,0,0,0];

current_f = fft([7,4,0,0], 4);
y_f = (current_f .* h1f) + (prev_f .* h2f);
y = ifft(y_f)(1:2)
prev_f = current_f;

current_f = fft([0,0,7,4], 4);
y_f = (current_f .* h1f) + (prev_f .* h2f);
y = ifft(y_f)(1:2)
prev_f = current_f;

current_f = fft([0,0,0,0], 4);
y_f = (current_f .* h1f) + (prev_f .* h2f);
y = ifft(y_f)(1:2)







