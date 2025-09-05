#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PASSO 0.1
#define X_MIN 0.0
#define X_MAX 20.0

double pertinencia_ruim(double x) {         //Triangular centrada em 0, mesma coisa para comida e serviço
    if (x <= 0.5) return 1.0 - 2.0 * x;
    return 0.0;
}

double pertinencia_boa(double x) {          //Triangular centrada em 0,5
    if (x >= 0.25 && x <= 0.75)
        return 1.0 - fabs(x - 0.5) * 4;
    return 0.0;
}

double pertinencia_saborosa(double x) {     //Triangular centrada em 1
    if (x >= 0.5) return (x - 0.5) * 2.0;
    return 0.0;
}

double pertinencia_aceitavel(double x) {
    if (x >= 0.25 && x <= 0.75)
        return 1.0 - fabs(x - 0.5) * 4;
    return 0.0;
}

double pertinencia_otimo(double x) {
    if (x >= 0.5) return (x - 0.5) * 2.0;
    return 0.0;
}

double gorjeta_pequena(double x) {
    if (x <= 0) return 1.0;
    else if (x < 5) return (5.0 - x) / 5.0;
    else if (x < 10) return (10.0 - x) / 5.0;
    else return 0.0;
}

double gorjeta_media(double x) {
    if (x < 5) return 0.0;
    else if (x < 10) return (x - 5.0) / 5.0;
    else if (x < 15) return (15.0 - x) / 5.0;
    else if (x < 17) return (17.0 - x) / 2.0;
    else return 0.0;
}

double gorjeta_alta(double x) {
    if (x < 12) return 0.0;
    else if (x < 17) return (x - 12.0) / 5.0;
    else if (x <= 20) return (20.0 - x) / 3.0;
    else return 0.0;
}

//Deffuzificação por centróide (aproximado por somatório):
//Saída = Sum(x * mu(x)) / Sum(mu(x))
// A cada x (valor de gorjeta de 0 a 20), calcula quanto cada regra contribui para aquele ponto e pega o máximo entre as contribuições

double defuzzificar(double g_pequena, double g_media, double g_alta) {
    double soma_num = 0.0, soma_den = 0.0;
    for (double x = X_MIN; x <= X_MAX; x += PASSO) {
        // Grau de ativação das funções de saída (corte pelas regras)
        double mu_p = fmin(g_pequena, gorjeta_pequena(x));
        double mu_m = fmin(g_media, gorjeta_media(x));
        double mu_a = fmin(g_alta, gorjeta_alta(x));

        double mu_total = fmax(fmax(mu_p, mu_m), mu_a); // agregação (max) - Vê qual regra prevalece mais

        soma_num += x * mu_total;
        soma_den += mu_total;
    }

    if (soma_den == 0.0) return 0.0; // evitar divisão por zero
    return soma_num / soma_den;
}

double calcula_gorjeta(double comida, double servico) {
    // Grau de pertinência para cada conjunto fuzzy
    double grau_ruim_comida = pertinencia_ruim(comida);
    double grau_boa_comida = pertinencia_boa(comida);
    double grau_saborosa_comida = pertinencia_saborosa(comida);

    double grau_ruim_servico = pertinencia_ruim(servico);
    double grau_aceitavel_servico = pertinencia_aceitavel(servico);
    double grau_otimo_servico = pertinencia_otimo(servico);

    //Concentração
    double grau_muito_ruim_comida = pow(grau_ruim_comida,6.0);
    double grau_muito_ruim_servico = pow(grau_ruim_servico,6.0);

    //Diluição


    // Regras:
    // 1. Se comida muito ruim OU serviço muito ruim → gorjeta pequena
    // 2. Se comida boa E serviço aceitável → gorjeta média
    // 3. Se comida saborosa E serviço aceitável → gorjeta média
    // 4. Se comida boa E serviço ótimo → gorjeta alta
    // 5. Se comida saborosa E serviço ótimo → gorjeta alta

    double regra1 = fmax(grau_ruim_comida, grau_ruim_servico);
    double regra2 = fmin(grau_boa_comida, grau_aceitavel_servico);
    double regra3 = fmin(grau_saborosa_comida, grau_aceitavel_servico);
    double regra4 = fmin(grau_boa_comida, grau_otimo_servico);
    double regra5 = fmin(grau_saborosa_comida, grau_otimo_servico);

    // Agregação por saída
    // Representação da saída como um conjunto Fuzzy
    double g_pequena = regra1;
    double g_media   = fmax(regra2, regra3);
    double g_alta    = fmax(regra4, regra5);

    //Deffuzificar
    return defuzzificar(g_pequena, g_media, g_alta);
}

int main(){
    FILE *fp = fopen("superficie.csv", "w");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    for (double comida = 0.0; comida <= 1.0; comida += PASSO) {
        for (double servico = 0.0; servico <= 1.0; servico += PASSO) {
            double gorjeta = calcula_gorjeta(comida, servico);
            fprintf(fp, "%.2f,%.2f,%.2f\n", comida, servico, gorjeta);
        }
    }

    fclose(fp);
    return 0;
}