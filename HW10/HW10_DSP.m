%%Open COM connection

mySerial = serial('/dev/tty.usbmodem1411');
fopen(mySerial);
fprintf(mySerial,'r'); % send 'r' to recieve data

data = zeros(100,4);
index = zeros(100,1);
rawData = zeros(100,1);
mafData = zeros(100,1);
iirData = zeros(100,1);


for i=1:100
    data(i,:) = (fscanf(mySerial, '%d %d %f %f'))';
end

fclose(mySerial);

index(:) = data(:,1);
rawData(:) = data(:,2);
mafData(:) = data(:,3);
iirData(:) = data(:,4);

sampleRate = 100;
numSamples = 100;
time = 0:1/sampleRate:numSamples/sampleRate-1/sampleRate;

figure
hold on
plot(time,rawData)
%plot(time,mafData)
%plot(time,iirData)
%legend('Raw Data', 'MAF', 'IIR');
xlabel('Time (s)')
ylabel('Z acceleration')
hold off

%plotFFT(rawData);