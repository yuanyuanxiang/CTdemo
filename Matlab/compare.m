if ~exist('yyx_radon')
    test;
end
yyx_radon2 = load('投影图像.txt');
yyx_reconstruct2 = load('重建图像.txt');

err1 = yyx_radon - yyx_radon2;
err2 = yyx_reconstruct - yyx_reconstruct2;

figure;
imshow(err1, []);
figure;
imshow(err2, []);