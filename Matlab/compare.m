if ~exist('yyx_radon')
    test;
end
yyx_radon2 = load('ͶӰͼ��.txt');
yyx_reconstruct2 = load('�ؽ�ͼ��.txt');

err1 = yyx_radon - yyx_radon2;
err2 = yyx_reconstruct - yyx_reconstruct2;

figure;
imshow(err1, []);
figure;
imshow(err2, []);