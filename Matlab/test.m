% ����MATLAB radon��iradon������ͼ�������ͶӰ�ͷ�ͶӰ

yuanyuanxiang = imread('Ԭ����.bmp');
[m, n] = size(yuanyuanxiang);
% 0��180�Ȳ���360��
theta = linspace(0, 180, 360);
yyx_radon = radon(yuanyuanxiang, theta);
figure;
imshow(yyx_radon, []);
title('Ԭ���� - ͶӰ');
dlmwrite('yyx_radon.txt', yyx_radon);
yyx_reconstruct = iradon(yyx_radon, theta, 'linear', 'Cosine', 1, m);
figure;
imshow(yyx_reconstruct, []);
title('Ԭ���� - �ؽ�');
dlmwrite('yyx_reconstruct.txt', yyx_reconstruct);