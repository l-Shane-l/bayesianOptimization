#include "bayesopt/bayesopt.hpp"

#include <Eigen/Dense>
#include <iostream>
#include <random>
#include <vector>
#include <iomanip>  // For std::setprecision
#include <fstream>

using namespace Eigen;
using namespace std;
using namespace bayesopt;

// Define the true signal as a sine wave
VectorXd generateTrueSignal(int size) {
    VectorXd signal(size);
    for (int i = 0; i < size; ++i) {
        signal(i) = sin(2 * M_PI * i / size);
    }
    return signal;
}

// Scramble the signal by adding noise
VectorXd scrambleSignal(const VectorXd &signal, double noise_level) {
    VectorXd noisy_signal = signal;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0, noise_level);

    for (int i = 0; i < signal.size(); ++i) {
        noisy_signal(i) += dis(gen);
    }
    return noisy_signal;
}

// Define the objective function for Bayesian Optimization
class SignalRecovery : public bayesopt::ContinuousModel {
public:
    SignalRecovery(const VectorXd &true_signal, const VectorXd &scrambled_signal, const bayesopt::Parameters &params)
        : ContinuousModel(true_signal.size(), params), true_signal_(true_signal), scrambled_signal_(scrambled_signal) {}

    double evaluateSample(const vectord &params) override {
        try {
            VectorXd recovered_signal(params.size());
            for (size_t i = 0; i < params.size(); ++i) {
                recovered_signal(i) = params(i);
            }
            double mse = (true_signal_ - recovered_signal).squaredNorm() / params.size();
            errors_.push_back(mse);
            printProgress(errors_.size(), mse);  // Print MSE in real time
            return -mse;  // Minimize negative MSE
        } catch (const std::exception &e) {
            std::cerr << "\nException caught during optimization: " << e.what() << std::endl;
            return std::numeric_limits<double>::infinity();  // Return a very large value to indicate failure
        }
    }

    const std::vector<double>& getErrors() const {
        return errors_;
    }

private:
    VectorXd true_signal_;
    VectorXd scrambled_signal_;
    std::vector<double> errors_;

    void printProgress(int iteration, double mse) const {
        cout << "\r\033[K" << "Iteration " << iteration << ": MSE = " << std::fixed << std::setprecision(4) << mse << " " << getProgressBar(mse) << flush;
    }

    string getProgressBar(double mse) const {
        const int barWidth = 50;
        double norm_mse = 1.0 / (1.0 + mse);  // Normalize the MSE to a value between 0 and 1
        int filledWidth = static_cast<int>(norm_mse * barWidth);
        string bar = "[";

        // Determine the color based on MSE
        string color;
        string icon;
        if (mse > 1.0) {
            color = "\033[31m";  // Red for bad (high MSE)
            icon = "❌";
        } else if (mse > 0.1) {
            color = "\033[33m";  // Orange for okay (medium MSE)
            icon = "⚠️";
        } else {
            color = "\033[32m";  // Green for good (low MSE)
            icon = "✅";
        }

        bar.append(color);  // Start color
        bar.append(filledWidth, '#');  // Use full block character
        bar.append("\033[0m");  // Reset color
        bar.append(barWidth - filledWidth, '_');  // Use light shade character for the empty part
        bar.append("]");
        bar.append(" ");
        bar.append(icon);  // Append the icon
        return bar;
    }
};

int main() {
    // Generate and scramble the signal
    int signal_size = 50;
    VectorXd true_signal = generateTrueSignal(signal_size);
    cout << "True Signal:\n" << true_signal.transpose() << endl;

    double noise_level = 0.1;
    VectorXd scrambled_signal = scrambleSignal(true_signal, noise_level);
    cout << "Scrambled Signal:\n" << scrambled_signal.transpose() << endl;

    // Set up Bayesian Optimization
    bayesopt::Parameters param = initialize_parameters_to_default();
    param.n_iterations = 100;

    // Create the optimization model
    SignalRecovery opt(true_signal, scrambled_signal, param);

    // Perform Bayesian optimization
    try {
        bayesopt::vectord result(signal_size); // Create a vectord object for the result
        opt.optimize(result);
    } catch (const std::exception &e) {
        std::cerr << "Optimization failed: " << e.what() << std::endl;
    }

    return 0;
}

