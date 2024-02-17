// G2_TP068579_TP068585_TP069261_LinkedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;

enum class SortColumn { Grams, Calories, Protein, Fat, SatFat, Fiber, Carbs };

struct FoodItem {
    string food = "";
    string measure = "";
    int grams = 0;
    int calories = 0;
    int protein = 0;
    int fat = 0;
    int satFat = 0;
    float fiber = 0.0f;
    float carbs = 0.0f;
    string category = "";
};

struct Node {
    FoodItem data;
    Node* next;
};

string cleanNumericField(string str) {
    str.erase(remove(str.begin(), str.end(), ','), str.end());
    str.erase(remove(str.begin(), str.end(), '\"'), str.end());
    return str.empty() || str.find_first_not_of("0123456789.-") != string::npos ? "0" : str;
}

FoodItem parseCSVLine(const string& line) {
    stringstream ss(line);
    FoodItem item;
    string temp;

    try {
        getline(ss, item.food, ',');
        getline(ss, item.measure, ',');
        getline(ss, temp, ','); item.grams = stoi(cleanNumericField(temp));
        getline(ss, temp, ','); item.calories = stoi(cleanNumericField(temp));
        getline(ss, temp, ','); item.protein = stoi(cleanNumericField(temp));
        getline(ss, temp, ','); item.fat = stoi(cleanNumericField(temp));
        getline(ss, temp, ','); item.satFat = stoi(cleanNumericField(temp));
        getline(ss, temp, ','); item.fiber = stof(cleanNumericField(temp));
        getline(ss, temp, ','); item.carbs = stof(cleanNumericField(temp));
        getline(ss, item.category);
    }
    catch (const std::invalid_argument& e) {
        cerr << "Error parsing line: " << line << "\nException message: " << e.what() << endl;
    }

    return item;
}

void addToList(Node*& head, FoodItem item) {
    Node* newNode = new Node{ item, nullptr };
    if (head == nullptr) {
        head = newNode;
    }
    else {
        Node* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void loadDatasetToLinkedList(Node*& head, const string& filename) {
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        getline(file, line); // Skip header
        while (getline(file, line)) {
            addToList(head, parseCSVLine(line));
        }
    }
    else {
        cout << "Unable to open file: " << filename << endl;
        return;
    }
}

bool compareFoodItems(const FoodItem& a, const FoodItem& b, SortColumn column, bool ascending) {
    switch (column) {
    case SortColumn::Grams:
        return ascending ? a.grams < b.grams : a.grams > b.grams;
    case SortColumn::Calories:
        return ascending ? a.calories < b.calories : a.calories > b.calories;
    case SortColumn::Protein:
        return ascending ? a.protein < b.protein : a.protein > b.protein;
    case SortColumn::Fat:
        return ascending ? a.fat < b.fat : a.fat > b.fat;
    case SortColumn::SatFat:
        return ascending ? a.satFat < b.satFat : a.satFat > b.satFat;
    case SortColumn::Fiber:
        return ascending ? a.fiber < b.fiber : a.fiber > b.fiber;
    case SortColumn::Carbs:
        return ascending ? a.carbs < b.carbs : a.carbs > b.carbs;
    default:
        return false;
    }
}

void bubbleSortLinkedList(Node*& head, SortColumn column, bool ascending) {
    if (head == nullptr || head->next == nullptr) {
        return;
    }

    bool swapped;
    Node* lastUnsorted = nullptr;

    do {
        swapped = false;
        Node* current = head;

        while (current->next != lastUnsorted) {
            if (!compareFoodItems(current->data, current->next->data, column, ascending)) {
                swap(current->data, current->next->data);
                swapped = true;
            }
            current = current->next;
        }
        lastUnsorted = current;
    } while (swapped);
}


Node* sortedMerge(Node* a, Node* b, SortColumn column, bool ascending) {
    Node* result = nullptr;

    if (a == nullptr)
        return b;
    else if (b == nullptr)
        return a;

    if (compareFoodItems(a->data, b->data, column, ascending)) {
        result = a;
        result->next = sortedMerge(a->next, b, column, ascending);
    }
    else {
        result = b;
        result->next = sortedMerge(a, b->next, column, ascending);
    }
    return result;
}

void frontBackSplit(Node* source, Node** frontRef, Node** backRef) {
    Node* fast;
    Node* slow;
    slow = source;
    fast = source->next;

    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}

void mergeSortLinkedList(Node** headRef, SortColumn column, bool ascending) {
    Node* head = *headRef;
    Node* a;
    Node* b;

    if ((head == nullptr) || (head->next == nullptr)) {
        return;
    }

    frontBackSplit(head, &a, &b);

    mergeSortLinkedList(&a, column, ascending);
    mergeSortLinkedList(&b, column, ascending);

    *headRef = sortedMerge(a, b, column, ascending);
}

void sortedInsert(Node** sortedRef, Node* newNode, SortColumn column, bool ascending) {
    Node* current;

    if (*sortedRef == nullptr || !compareFoodItems((*sortedRef)->data, newNode->data, column, ascending)) {
        newNode->next = *sortedRef;
        *sortedRef = newNode;
    }
    else {
        current = *sortedRef;
        while (current->next != nullptr && compareFoodItems(current->next->data, newNode->data, column, ascending)) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void insertionSortLinkedList(Node** headRef, SortColumn column, bool ascending) {
    Node* sorted = nullptr;

    Node* current = *headRef;
    while (current != nullptr) {
        Node* next = current->next;

        sortedInsert(&sorted, current, column, ascending);

        current = next;
    }

    *headRef = sorted;
}

void displayList(Node* head) {
    if (head == nullptr) {
        cout << "Linked list is empty." << endl;
        return;
    }

    cout << left << setw(20) << "Food" << setw(15) << "Measure" << setw(8) << "Grams"
        << setw(10) << "Calories" << setw(10) << "Protein" << setw(8) << "Fat"
        << setw(10) << "Sat.Fat" << setw(10) << "Fiber" << setw(10) << "Carbs"
        << setw(20) << "Category" << endl;
    cout << string(111, '-') << endl; // Line separator

    Node* current = head;
    while (current != nullptr) {
        cout << left << setw(20) << (current->data.food.length() > 10 ? current->data.food.substr(0, 10) + "..." : current->data.food)
            << setw(15) << (current->data.measure.length() > 10 ? current->data.measure.substr(0, 10) + "..." : current->data.measure)
            << setw(8) << current->data.grams
            << setw(10) << current->data.calories
            << setw(10) << current->data.protein
            << setw(8) << current->data.fat
            << setw(10) << current->data.satFat
            << setw(10) << current->data.fiber
            << setw(10) << current->data.carbs
            << setw(20) << (current->data.category.length() > 10 ? current->data.category.substr(0, 10) + "..." : current->data.category) << endl;
        current = current->next;
    }
}




void freeLinkedList(Node*& head) {
    Node* temp;
    while (head != nullptr) {
        temp = head;
        head = head->next;
        delete temp;
    }
}

void sortLinkedList(Node*& head, SortColumn column, bool ascending, int sortAlgorithmChoice) {
    switch (sortAlgorithmChoice) {
    case 0:
        cout << "Performing Bubble Sort..." << endl;
        bubbleSortLinkedList(head, column, ascending);
        break;
    case 1:
        cout << "Performing Merge Sort..." << endl;
        mergeSortLinkedList(&head, column, ascending);
        break;
    case 2:
        cout << "Performing Insertion Sort..." << endl;
        insertionSortLinkedList(&head, column, ascending);
        break;
    default:
        cerr << "Invalid sorting algorithm choice." << endl;
    }
}



int main() {
    Node* head = nullptr;
    string filename = "Nutrients_Info_Modified.csv";

    loadDatasetToLinkedList(head, filename);

    while (true) {
        //system("cls"); // Clear the screen for better readability

        cout << "-----------------------------------" << endl;
        cout << "|     Linked List Sorting         |" << endl;
        cout << "-----------------------------------" << endl;
        cout << "This program sorts food items in a linked list based on selected criteria." << endl << endl;

        SortColumn column = SortColumn::Grams;
        bool ascending = true;
        int columnChoice, orderChoice, sortAlgorithmChoice;

        cout << "Select column to sort by (0: Grams, 1: Calories, 2: Protein, 3: Fat, 4: SatFat, 5: Fiber, 6: Carbs): ";
        cin >> columnChoice;
        column = static_cast<SortColumn>(columnChoice);

        cout << "Select sort order (0: Descending, 1: Ascending): ";
        cin >> orderChoice;
        ascending = orderChoice != 0;

        cout << "Select sorting algorithm (0: Bubble Sort, 1: Merge Sort, 2: Insertion Sort): ";
        cin >> sortAlgorithmChoice;

        auto start = chrono::high_resolution_clock::now();

        sortLinkedList(head, column, ascending, sortAlgorithmChoice);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> sortingTime = end - start;

        cout << "\nSorted Linked List:\n";
        displayList(head);

        cout << fixed << setprecision(5) << "\nTotal sorting time: " << sortingTime.count() << " seconds\n";

        cout << "\nPress any key to continue...";
        cin.ignore();
        cin.get();
    }

    freeLinkedList(head);

    return 0;
}

