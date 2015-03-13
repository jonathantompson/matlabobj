clearvars; clc; close all;

bunny = obj_read('bunny.obj');

figure;
set(gcf, 'Position', [200 200 1200 1200]);
trimesh(bunny.Tri'+1, bunny.Position(1,:), bunny.Position(3,:), bunny.Position(2,:));
hold on;
set(gcf,'renderer','opengl'); axis vis3d; axis equal;