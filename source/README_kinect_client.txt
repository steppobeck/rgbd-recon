in build/Release
ln -s ../../../glsl .
ln -s /opt/kinect-resources/rgbd-framework/recordings/stepptanz/ stepptanz

copy this content into file 
build/Release/stepptanz.ksV3 :

serverport 127.0.0.1:7000
kinect stepptanz/23.yml
kinect stepptanz/24.yml
kinect stepptanz/25.yml
kinect stepptanz/26.yml

# Play stepptanz:
cd /opt/kinect-resources/rgbd-framework/rgbd-calib/build/build/Release
./play -c -f 20 -k 4 ../../../../recordings/stepptanz/stepptanz.stream 127.0.0.1:7000
# Run kinect_client:
./kinect_client stepptanz.ksV3



