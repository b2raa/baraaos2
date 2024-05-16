#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <iomanip>
#include <climits>
#include <queue>
using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int start_time;
    int remaining_time;
};

list<Process> readProcesses(const string& filename) {
    ifstream file(filename);
    list<Process> processes;
    int pid, arrival_time, burst_time;
    while (file >> pid >> arrival_time >> burst_time) {
        processes.push_back({pid, arrival_time, burst_time, 0, 0, 0, 0, 0, burst_time});
    }
    file.close();
    return processes;
}

void FCFS(list<Process>& processes, int context_switch_time) {
    int current_time = 0;
    for (auto& process : processes) {
        process.start_time = max(current_time, process.arrival_time);
        current_time = process.start_time + process.burst_time;
        process.completion_time = current_time;
        process.turnaround_time = process.completion_time - process.arrival_time;
        process.waiting_time = process.turnaround_time - process.burst_time;
        process.response_time = process.start_time - process.arrival_time;
        current_time += context_switch_time;
    }
}

void SRT(list<Process>& processes, int context_switch_time) {
    int current_time = 0;
    int completed = 0;
    while (completed < processes.size()) {
        int shortest_index = -1;
        int shortest_remaining_time = INT_MAX;
        for (auto it = processes.begin(); it != processes.end(); ++it) {
            if (it->arrival_time <= current_time && it->remaining_time < shortest_remaining_time && it->remaining_time > 0) {
                shortest_remaining_time = it->remaining_time;
                shortest_index = distance(processes.begin(), it);
            }
        }
        if (shortest_index == -1) {
            current_time++;
        } else {
            auto it = next(processes.begin(), shortest_index);
            Process& current_process = *it;
            current_process.start_time = current_time;
            current_process.remaining_time--;
            current_time++;
            if (current_process.remaining_time == 0) {
                current_process.completion_time = current_time;
                current_process.turnaround_time = current_process.completion_time - current_process.arrival_time;
                current_process.waiting_time = current_process.turnaround_time - current_process.burst_time;
                current_process.response_time = current_process.start_time - current_process.arrival_time;
                completed++;
                current_time += context_switch_time;
            }
        }
    }
}

void RR(list<Process>& processes, int context_switch_time, int quantum) {
    queue<Process*> ready_queue;
    int current_time = 0;
    int remaining_processes = processes.size();
    auto it = processes.begin();

    while (remaining_processes > 0) {
        while (it != processes.end() && it->arrival_time <= current_time) {
            ready_queue.push(&(*it));
            ++it;
        }

        if (ready_queue.empty()) {
            current_time++;
            continue;
        }

        Process* current_process = ready_queue.front();
        ready_queue.pop();

        current_process->start_time = current_time;
        int execution_time = min(quantum, current_process->remaining_time);
        current_time += execution_time;
        current_process->remaining_time -= execution_time;

        if (current_process->remaining_time == 0) {
            current_process->completion_time = current_time;
            current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
            current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;
            current_process->response_time = current_process->start_time - current_process->arrival_time;
            remaining_processes--;
        } else {
            while (it != processes.end() && it->arrival_time <= current_time) {
                ready_queue.push(&(*it));
                ++it;
            }
            ready_queue.push(current_process);
        }

        current_time += context_switch_time;
    }
}

void displayResults(const list<Process>& processes) {
    float total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0;
    float cpu_utilization = 0;
    cout << "Process No. | Arrival Time | Burst Time | Completion Time | Turnaround Time | Waiting Time | Response Time" << endl;
    cout << "------------------------------------------------------------------------------------------------------" << endl;
    for (const auto& process : processes) {
        cout << setw(11) << process.pid << " | ";
        cout << setw(12) << process.arrival_time << " | ";
        cout << setw(10) << process.burst_time << " | ";
        cout << setw(15) << process.completion_time << " | ";
        cout << setw(15) << process.turnaround_time << " | ";
        cout << setw(12) << process.waiting_time << " | ";
        cout << setw(13) << process.response_time << endl;

        total_waiting_time += process.waiting_time;
        total_turnaround_time += process.turnaround_time;
        total_response_time += process.response_time;
    }

    cout << "------------------------------------------------------------------------------------------------------" << endl;
    cout << "Average Turnaround Time: " << total_turnaround_time / processes.size() << endl;
    cout << "Average Waiting Time: " << total_waiting_time / processes.size() << endl;
    cout << "Average Response Time: " << total_response_time / processes.size() << endl;
    cpu_utilization = (total_turnaround_time - total_waiting_time) / processes.back().completion_time * 100;
    cout << "CPU Utilization: " << cpu_utilization << "%" << endl;
}

int main() {
    list<Process> processes;
    string filename = "/Users/salamcyb/Desktop/c.txt";
    processes = readProcesses(filename);
    int context_switch_time = 1;
    int quantum = 2;

    list<Process> fcfs_processes = processes;
    fcfs_processes.sort([](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
    FCFS(fcfs_processes, context_switch_time);
    cout << "FCFS Schedule:\n";
    displayResults(fcfs_processes);

    list<Process> srt_processes = processes;
    srt_processes.sort([](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
    SRT(srt_processes, context_switch_time);
    cout << "\nSRT Schedule:\n";
    displayResults(srt_processes);

    list<Process> rr_processes = processes;
    rr_processes.sort([](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
    RR(rr_processes, context_switch_time, quantum);
    cout << "\nRound Robin Schedule:\n";
    displayResults(rr_processes);

    return 0;
}

/*#include <iostream> // Include the input/output stream library.
 #include <fstream> // Include the file stream library for file operations.
 #include <list> // Include the list container library.
 #include <algorithm> // Include the algorithm library for STL algorithms.
 #include <iomanip> // Include the input/output manipulators library for formatting.
 #include <climits> // Include the limits library for integer limits.
 #include <queue> // Include the queue container library.
 using namespace std; // Using the standard namespace.

 // Define a structure to represent a process.
 struct Process {
     int pid; // Process ID.
     int arrival_time; // Arrival time of the process.
     int burst_time; // Burst time required by the process.
     int completion_time; // Time when the process completes execution.
     int waiting_time; // Waiting time of the process.
     int turnaround_time; // Turnaround time of the process.
     int response_time; // Response time of the process.
     int start_time; // Start time of the process.
     int remaining_time; // Remaining execution time of the process.
 };

 // Function to read processes from a file and return them as a list.
 list<Process> readProcesses(const string& filename) {
     ifstream file(filename); // Open the file for reading.
     list<Process> processes; // Initialize an empty list to store processes.
     int pid, arrival_time, burst_time; // Variables to store process attributes.
     // Read process attributes from the file and add them to the list.
     while (file >> pid >> arrival_time >> burst_time) {
         processes.push_back({pid, arrival_time, burst_time, 0, 0, 0, 0, 0, burst_time});
     }
     file.close(); // Close the file.
     return processes; // Return the list of processes.
 }

 // Function to schedule processes using First-Come, First-Served (FCFS) algorithm.
 void FCFS(list<Process>& processes, int context_switch_time) {
     int current_time = 0; // Initialize the current time to 0.
     // Iterate through each process in the list.
     for (auto& process : processes) {
         process.start_time = max(current_time, process.arrival_time); // Set start time of the process.
         current_time = process.start_time + process.burst_time; // Update current time.
         // Calculate completion time, turnaround time, waiting time, and response time.
         process.completion_time = current_time;
         process.turnaround_time = process.completion_time - process.arrival_time;
         process.waiting_time = process.turnaround_time - process.burst_time;
         process.response_time = process.start_time - process.arrival_time;
         current_time += context_switch_time; // Add context switch time.
     }
 }

 // Function to schedule processes using Shortest Remaining Time (SRT) algorithm.
 void SRT(list<Process>& processes, int context_switch_time) {
     int current_time = 0; // Initialize the current time to 0.
     int completed = 0; // Counter to track completed processes.
     // Continue scheduling until all processes are completed.
     while (completed < processes.size()) {
         int shortest_index = -1; // Index of the process with the shortest remaining time.
         int shortest_remaining_time = INT_MAX; // Initialize shortest remaining time to maximum.
         // Find the process with the shortest remaining time that has arrived.
         for (auto it = processes.begin(); it != processes.end(); ++it) {
             if (it->arrival_time <= current_time && it->remaining_time < shortest_remaining_time && it->remaining_time > 0) {
                 shortest_remaining_time = it->remaining_time;
                 shortest_index = distance(processes.begin(), it);
             }
         }
         // If no process is found, increment current time.
         if (shortest_index == -1) {
             current_time++;
         } else {
             auto it = next(processes.begin(), shortest_index); // Get iterator to the shortest process.
             Process& current_process = *it; // Reference to the shortest process.
             current_process.start_time = current_time; // Set start time of the process.
             current_process.remaining_time--; // Decrement remaining time.
             current_time++; // Increment current time.
             // If process is completed, calculate metrics and update counters.
             if (current_process.remaining_time == 0) {
                 current_process.completion_time = current_time;
                 current_process.turnaround_time = current_process.completion_time - current_process.arrival_time;
                 current_process.waiting_time = current_process.turnaround_time - current_process.burst_time;
                 current_process.response_time = current_process.start_time - current_process.arrival_time;
                 completed++;
                 current_time += context_switch_time; // Add context switch time.
             }
         }
     }
 }

 // Function to schedule processes using Round Robin (RR) algorithm.
 void RR(list<Process>& processes, int context_switch_time, int quantum) {
     queue<Process*> ready_queue; // Queue to store ready processes.
     int current_time = 0; // Initialize the current time to 0.
     int remaining_processes = processes.size(); // Counter to track remaining processes.
     auto it = processes.begin(); // Iterator to iterate through processes.

     // Continue scheduling until all processes are completed.
     while (remaining_processes > 0) {
         // Add processes arriving at current time to the ready queue.
         while (it != processes.end() && it->arrival_time <= current_time) {
             ready_queue.push(&(*it));
             ++it;
         }

         // If no process is ready, increment current time.
         if (ready_queue.empty()) {
             current_time++;
             continue;
         }

         Process* current_process = ready_queue.front(); // Get the first process from the ready queue.
         ready_queue.pop(); // Remove the process from the queue.

         current_process->start_time = current_time; // Set start time of the process.
         int execution_time = min(quantum, current_process->remaining_time); // Determine execution time.
         current_time += execution_time; // Update current time.
         current_process->remaining_time -= execution_time; // Update remaining time.

         // If process is completed, calculate metrics and update counters.
         if (current_process->remaining_time == 0) {
             current_process->completion_time = current_time;
             current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
             current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;
             current_process->response_time = current_process->start_time - current_process->arrival_time;
             remaining_processes--;
         } else {
             // Add processes arriving during execution to the ready queue.
             while (it != processes.end() && it->arrival_time <= current_time) {
                 ready_queue.push(&(*it));
                 ++it;
             }
             ready_queue.push(current_process); // Add current process back to the queue.
         }

         current_time += context_switch_time; // Add context switch time.
     }
 }

 // Function to display scheduling results.
 void displayResults(const list<Process>& processes) {
     float total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0; // Initialize metrics.
     float cpu_utilization = 0; // Initialize CPU utilization.
     // Display header for results table.
     cout << "Process No. | Arrival Time | Burst Time | Completion Time | Turnaround Time | Waiting Time | Response Time" << endl;
     cout << "------------------------------------------------------------------------------------------------------" << endl;
     // Iterate through each process and display its metrics.
     for (const auto& process : processes) {
         cout << setw(11) << process.pid << " | ";
         cout << setw(12) << process.arrival_time << " | ";
         cout << setw(10) << process.burst_time << " | ";
         cout << setw(15) << process.completion_time << " | ";
         cout << setw(15) << process.turnaround_time << " | ";
         cout << setw(12) << process.waiting_time << " | ";
         cout << setw(13) << process.response_time << endl;

         // Update total metrics.
         total_waiting_time += process.waiting_time;
         total_turnaround_time += process.turnaround_time;
         total_response_time += process.response_time;
     }

     // Display footer with average metrics and CPU utilization.
     cout << "------------------------------------------------------------------------------------------------------" << endl;
     cout << "Average Turnaround Time: " << total_turnaround_time / processes.size() << endl;
     cout << "Average Waiting Time: " << total_waiting_time / processes.size() << endl;
     cout << "Average Response Time: " << total_response_time / processes.size() << endl;
     cpu_utilization = (total_turnaround_time - total_waiting_time) / processes.back().completion_time * 100;
     cout << "CPU Utilization: " << cpu_utilization << "%" << endl;
 }

 // Main function
 int main() {
     list<Process> processes; // Initialize a list to store processes.
     string filename = "/Users/salamcyb/Desktop/c.txt"; // Path to the input file.
     processes = readProcesses(filename); // Read processes from the file.
     int context_switch_time = 1; // Time taken for context switch.
     int quantum = 2; // Quantum time for Round Robin algorithm.

     // Schedule processes using First-Come, First-Served algorithm.
     list<Process> fcfs_processes = processes;
     fcfs_processes.sort([](const Process& a, const Process& b) {
         return a.arrival_time < b.arrival_time;
     });
     FCFS(fcfs_processes, context_switch_time);
     cout << "FCFS Schedule:\n";
     displayResults(fcfs_processes);

     // Schedule processes using Shortest Remaining Time algorithm.
     list<Process> srt_processes = processes;
     srt_processes.sort([](const Process& a, const Process& b) {
         return a.arrival_time < b.arrival_time;
     });
     SRT(srt_processes, context_switch_time);
     cout << "\nSRT Schedule:\n";
     displayResults(srt_processes);

     // Schedule processes using Round Robin algorithm.
     list<Process> rr_processes = processes;
     rr_processes.sort([](const Process& a, const Process& b) {
         return a.arrival_time < b.arrival_time;
     });
     RR(rr_processes, context_switch_time, quantum);
     cout << "\nRound Robin Schedule:\n";
     displayResults(rr_processes);

     return 0; // Return 0 to indicate successful execution.
 }
*/
