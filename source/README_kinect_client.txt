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
vshader glsl/rgb_depth_mappingV2.vs
gshader glsl/rgb_depth_mappingV2.gs
fshader glsl/rgb_depth_mappingV2.fs
hostname pandora
hostname achill
hostname demeter
hostname apollo
hostname orpheus
hostname medusa
hostname andromeda
hostname vr19
hostname minos
hostname agenor
hostname hektor
hostname arachne
hostname minos
hostname boreas
hostname athena
hostname nestor
hostname charon
hostname hades

# Play stepptanz:
cd /opt/kinect-resources/rgbd-framework/rgbd-calib/build/build/Release
./play -c -f 20 -k 4 ../../../../recordings/stepptanz/stepptanz.stream 127.0.0.1:7000
# Run kinect_client:
./kinect_client stepptanz.ksV3



