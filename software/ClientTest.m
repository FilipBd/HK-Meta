clear all
close all

fprintf('Opening socket...\n');
port = 30000;

t = tcpip('localhost', port, 'NetworkRole', 'client'); 
fprintf('Port %i is open\n', port);
fprintf('Waiting for connection...\n')
fopen(t);
framrate=60; %the fake framerate of the simulator 
quit = 0;

fprintf('spamming\n')
while(1)
    Xdata = rand(1);
    Ydata = rand(1);
    Zdata = rand(1);
    data = strcat(num2str(round(Xdata,3)),':',num2str(round(Ydata,3)),':',num2str(round(Zdata,3)));
    fprintf(t,data);
    pause(1/framerate)
end

fprintf('Done')
fclose(t); %closes the port
clear t;