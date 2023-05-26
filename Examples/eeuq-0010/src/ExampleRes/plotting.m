figid = 200;


names = {"1-PFA-0-1", "1-PFA-0-2", "1-PFA-1-1", "1-PFA-1-2", "1-PFD-1-1", "1-PFD-1-2", "1-PID-1-1", "1-PFA-2-1", "1-PFA-2-2", "1-PFD-2-1", "1-PFD-2-2", "1-PID-2-1", "1-PFA-3-1", "1-PFA-3-2", "1-PFD-3-1", "1-PFD-3-2", "1-PID-3-1", "1-PRD-1-1"}
idPSA = [1,2,3,4,8,9,13,14];
idPFD = [5,6,10,11,15,16];
idPID = [7,12,17];


sur = readmatrix("sur.csv",'NumHeaderLines',1);
sim = readmatrix("sim2.csv",'NumHeaderLines',1);

sur_res =sur(:,9:9+17);
sim_res =sim(:,4:4+17);


mediansur = (mean(log(sur_res)));
mediansim = (mean(log(sim_res)));
stdsur = (std(log(sur_res)));
stdsim = (std(log(sim_res)));

figure(figid+1)
% t = tiledlayout('flow');

% Team 1
% nexttile([2 5])
bar([mediansim;mediansur]')
ylabel("Mean of log EDP")
grid on;
set(gcf,'color','w')
% set(gca,'XTickLabel',names);
xticks(1:length(names))
xticklabels(names);
legend('original','surrogate');
title('Mean of log EDP')
% title('Log-scale')

set(gca,'fontsize',15)
fontname("times new roman")
% 
% nexttile([2 3])
% bar(exp([mediansim(idPSA);mediansur(idPSA)]'))
% % ylabel("Median PSA")
% ylabel("median PSA")
% grid on;
% set(gcf,'color','w')
% set(gca,'XTickLabel',{names{idPSA}});
% % legend('original','surrogate')
% title("Original scale (PSA)")
% set(gca,'fontsize',15)
% fontname("times new roman")
% 
% nexttile([2 2])
% bar(exp([mediansim(idPFD);mediansur(idPFD)]'))
% % ylabel("Median PID")
% ylabel("median PID")
% grid on;
% set(gcf,'color','w')
% set(gca,'XTickLabel',{names{idPFD}});
% % legend('original','surrogate')
% ylabel("median PID")
% title("Original scale (PFD)")
% % ylim([0 6e-3])
% set(gca,'fontsize',15)
% fontname("times new roman")
%%

set(gca,'fontsize',15)
fontname("times new roman")

figure(figid+2);
bar([stdsim;stdsur]')
ylabel("std[logEDP]")
grid on;
set(gcf,'color','w')
xticks(1:length(names))
xticklabels(names);
legend('original','surrogate')
title('Std. of log EDP')

set(gca,'fontsize',15)
fontname("times new roman")

%%

% figure(4)
% bar(exp([mediansim(idPID);mediansur(idPID)]'))
% % ylabel("Median PID")
% ylabel("median PID")
% grid on;
% set(gcf,'color','w')
% set(gca,'XTickLabel',{names{idPID}});
% % legend('original','surrogate')
% ylabel("median PID")
% title("Original scale (PFD)")
% % ylim([0 6e-3])
% set(gca,'fontsize',15)
% fontname("times new roman")

% 
% 
% b = bar([nonlinearPID, linearPID],'FaceColor','flat');
% % ylabel("Median PID")
% ylabel("median PID")
% grid on;
% set(gcf,'color','w')
% set(gca,'XTickLabel',{names{idPID}});
% % legend('original','surrogate')
% ylabel("median PID")
% title("Original scale (PFD)")
% % ylim([0 6e-3])
% set(gca,'fontsize',15)
% fontname("times new roman")
% 
% 
% 
% b(1).CData = 	[0.8 0  0.2];
% b(2).CData = 	[0.6 0  0.2];
% b(3).CData = 	[0 0.8 0.2];
% b(4).CData = 	[0 0.6 0.2];
% legend('linear-original','linear-surrogate','nonlinear-original','nonlinear-surrogate')
% 

