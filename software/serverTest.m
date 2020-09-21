clear all
close all

fprintf('Opening socket...\n');
port = 31090;

t = tcpip('0.0.0.0', port, 'NetworkRole', 'server'); %This will accept any TCP request on port .
fprintf('Port %i is open \n', port);
fprintf('Waiting for connection...\n')
fopen(t);

% Setting up the plot outside the loop, for greater speed
f = figure();
f.Units = 'normalized';
quit.Units = 'normalized';
f.Name = 'Acceleration Vectors';
movegui(f,'center');
f.Visible = 'on';

while(1)
     data = fscanf(t);
     split = strsplit(data,':');
     gx = str2double(split(1));
     gy = str2double(split(2));
     gz = str2double(split(3));
    cla;    %clear everything from the current axis
    
    %plot X acceleration vector
    line([0 gx], [0 0], [0 0], 'Color', 'r', 'LineWidth', 2, 'Marker', 'o');
    
    %plot Y acceleration vector
    line([0 0], [0 gy], [0 0], 'Color', 'b', 'LineWidth', 2, 'Marker', 'o');
    
    %plot Z acceleration vector
    line([0 0], [0 0], [0 gz], 'Color', 'g', 'LineWidth', 2, 'Marker', 'o');
    
    % resultant
    line([0 gx], [0 gy], [0 gz], 'Color', 'k', 'LineWidth', 2, 'Marker', 'o');
    
    drawnow limitrate;
    
end

fprintf('Done')
fclose(t); %closes the port
clear t;