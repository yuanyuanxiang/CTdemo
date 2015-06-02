% 调用MATLAB radon和iradon函数对图像进行正投影和反投影

yuanyuanxiang = imread('袁沅祥.bmp');
[m, n] = size(yuanyuanxiang);
% 0到180度采样360次
theta = linspace(0, 180, 360);
yyx_radon = radon(yuanyuanxiang, theta);
figure;
imshow(yyx_radon, []);
title('袁沅祥 - 投影');
dlmwrite('yyx_radon.txt', yyx_radon);
yyx_reconstruct = iradon(yyx_radon, theta, 'linear', 'Cosine', 1, m);
figure;
imshow(yyx_reconstruct, []);
title('袁沅祥 - 重建');
dlmwrite('yyx_reconstruct.txt', yyx_reconstruct);