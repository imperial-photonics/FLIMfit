E = linspace(0,1,1000);

nu = 1;

F = 3 * 2 * nu^6;
G = sqrt(E ./ (F .* (1 - E)));
H = 1 ./ (2 * (1 - E) .* sqrt(3 .* E .* F .* (1-E)));

Fd = 3 / 2 * E ./ (1 - E);

p = zeros(size(E));

sel = E < F ./ (1 + F);
p(sel) = H(sel) * log(2 + sqrt(3));

sel = (E > F ./ (1 + F)) & (E < 4 * F ./ (1 + 4 * F));
p(sel) = H(sel) .* log((2 + sqrt(3)) ./ (G(sel) + sqrt(G(sel).^2 - 1)));

plot(Fd,p)

%% 

nu = 2;

n = 10000;

x = rand(n);
z = rand(n);

nud = (1 + 3 * x.^2) .* z.^2 .* nu;

E = 1 ./ (1 + nud);

histogram(nud);

%%
f = linspace(0,4,1000);

p = (log(2 + sqrt(3)) - heaviside(f - 1) .* log(sqrt(f) + sqrt(f-1))) ./ (2 * sqrt(3 * f));
plot(f,p)

%%


G = 1.2;
x = linspace(0,1,1000);
z = G ./ sqrt(1 + 3 * x.^2);
plot(x,z);
ylim([0,1])


%%

x = linspace(0,10,1000);
y = 1./sqrt(x) .* erf(sqrt(x));
plot(x,y)

%% 

x = linspace(1,4,1000);
y = -log(sqrt(x)-sqrt(x-1))./sqrt(x);
plot(x,y)