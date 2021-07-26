#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <windows.h>
#include <mysql.h>
#include <sstream> // for stringstream
#include <ctime> // to measure time elapsed
#include <cctype>
#include <algorithm>
using namespace std;

const char* hostname = "localhost";
const char* username = "root";
const char* password = "...";
const char* database = "knowledge_testing_app";
unsigned int port = 3306;
const char* unixsocket = NULL;
unsigned long clientflag = 0;

MYSQL* connectdatabase() {
    MYSQL* connection;
    connection = mysql_init(NULL);
    connection = mysql_real_connect(connection, hostname, username, password, database, port, unixsocket, clientflag);
    if (connection)
    {
        MYSQL_RES* res;
        res = mysql_store_result(connection);
        int cp1251_status = mysql_query(connection, "SET NAMES 'cp1251'");
        if (!cp1251_status) {
            mysql_free_result(res);
        }
        else {
            cout << "��������� �� ��������\n";
        }
        return connection;
    }
    else {
        cout << "���������� � �� � MySQL �� �����������" << '\n';
        return 0;
    }
}


class Question {
private:
    int right_answer_SC; //  right answer for single-choice question (SCQ)
    vector<char> right_answers_MC; // right answer for multiple-choice question (MCQ)
    const char* right_word_input; // right answer for word input question (WIQ)
    clock_t start, finish; // timer
    int corr; //  returns a share of correct answers
public:
    Question () {}

    int ques_num = 1; // sequential number of question - for continuous numbering
    int corr_ans_iter = 0; // count of correct answers
    int ans_count = 0; // count of all answers obtained

    // method to get 1 answer in SCQ, called for from fetch_data_SC()
    char getOneAnswer() {
        cout << "\n    ������� ����� ������ ������: " << '\n';
        char n;
        cin >> n;
        return n;
    }

    // method to get 2 answers in MCQ, called for from fetch_data_MC()
    vector<char> getMultAnswers() {
        cout << "\n    ������� ��� ������ ����� �������: " << '\n';
        vector<char> v;
        while (v.size() < 2) {
            char temp;
            cin >> temp;
            v.push_back(temp);
        }

        return v;
    }

    // method to get word/abbreviation input (WIQ) from user
   const char* getUserInput() {
        cout << "\n    ����������� ��� �����: " << '\n';
        string a;
        getline(cin, a);
        const char* aa = a.c_str();
        return aa;
    }

    // fetch data for "word" user input (WIQ)
    int fetch_data_WI(MYSQL* connection) {
        MYSQL_ROW row;
        MYSQL_RES* result;
        if(connection) {
            int qstatus = mysql_query(connection, "SELECT * FROM word_input");
            result = mysql_store_result(connection);
            if(!qstatus) {
                int query_col_count = mysql_num_fields(result); // number of fields\columns in the above SELECT query
                start = clock(); // timer initialized
                cout << "\n\n\n//////////////////////////\n\n\n ���� �������� 'A' ������� �� ��� ���������� ����������� �����. ��� ����� ���� ���� ����� ��� �������� � �������, ��� ������������." << '\n';
                while((row = mysql_fetch_row(result))!=0){ // while row is not NULL
                    for(int c_iter = 0; c_iter < query_col_count; c_iter++) { // output column by column in each row
                        if (c_iter == 0) {
                                cout << "\n\n\n\n\n\n\n\n\n\n----------------------\n----------------------\n\n   ������ � :  " << ques_num << '\n';
                                ques_num++;
                                c_iter++; // in order to skip right-answer output
                        }
                        if (c_iter == query_col_count - 2) { // a column before last in row
                            this-> right_word_input = row[c_iter]; // now it is a const char*
                            if (*getUserInput() == *right_word_input) { // if under the 2 pointers are equal data sequences...
                                cout << "\n �� �������� �����.\n";
                                corr_ans_iter++; // adding to the number of correct answers
                                }
                            else {
                                cout << "\n �� ��������.\n";
                            }
                            c_iter++; // skiping right-answer output
                        }
                        if (c_iter == query_col_count - 1) {cout << "\n\n   ��������� � ������� ������ :\n\n";}
                        cout << row[c_iter]  << '\n';
                    }
                    ans_count++; // accumulating final answers count
                    cout << "\n\n\n ����� ������ ������ ������� � ���������� �������, ������� ������.\n";
                    system("pause");
                }
            }
            else {
                cout << "mysql_query error";
            }
            mysql_free_result(result); // obligatory at the end of mysql_store_result block
        }
        else {
        cout << "connection for fetch_data_SC failed";
        }

        return corr_ans_iter; // returns count of correct answers, then fetch_data_SC uses it
    }


// fetch data for single choice question (SCQ)
    int fetch_data_SC(MYSQL* connection) {
        MYSQL_ROW row;
        MYSQL_RES* result;
        if(connection) {
            int qstatus = mysql_query(connection, "SELECT * FROM single_choice");
            result = mysql_store_result(connection);
            if(!qstatus) {
                int query_col_count = mysql_num_fields(result);
                cout << "\n\n//////////////////////////\n\n ���� �������� 'B' ������� �� ��� 1 (����) ������� ������." << '\n';
                while((row = mysql_fetch_row(result))!=0){ 
                    for(int c_iter = 0; c_iter < query_col_count; c_iter++) { 
                        if (c_iter == 0) {
                                cout << "\n\n\n\n\n----------------------\n----------------------\n\n   ������ � :  " << ques_num << '\n';
                                ques_num++;
                                c_iter++; 
                        }
                        if (c_iter == query_col_count - 2) { 
                            this-> right_answer_SC = *row[c_iter];
                            if (getOneAnswer() == right_answer_SC) { // other method call + fetch data from pointer to this data
                                cout << "\n �� �������� �����.\n";
                                corr_ans_iter++; 
                                }
                            else {
                                cout << "\n �� ��������.\n";
                            }
                            c_iter++;
                        }
                        if (c_iter == query_col_count - 1)
                            {cout << "\n\n  ��������� � ������� ������ :\n\n";};

                        cout << row[c_iter]  << '\n';
                    }
                    ans_count++; // accumulating SCQ count
                    cout << "\n\n\n ����� ������ ������ ������� � ���������� �������, ������� ������.\n";
                    system("pause");
                }
            }
            else {
                cout << "mysql_query error";
            }
            mysql_free_result(result); 
        }
        else {
        cout << "connection for fetch_data_SC failed";
        }

        return corr_ans_iter; // returns count of correct answers, then fetch_data_MC uses it
    }

// fetch data for multiple choice question (MCQ)
    int fetch_data_MC(MYSQL* connection) {
        MYSQL_ROW row;
        MYSQL_RES* result;
        if(connection) {
            int qstatus = mysql_query(connection, "SELECT * FROM  multiple_choice");
            result = mysql_store_result(connection);
            if(!qstatus) {
                int query_col_count = mysql_num_fields(result); 
                vector<char> mult_answers;
                cout << "\n\n\n//////////////////////////\n\n\n ���� �������� 'C' ������� �� ��� 2 (���) �������� ������." << '\n';
                while((row = mysql_fetch_row(result))!=0){ 
                    for(int c_iter = 0; c_iter < query_col_count; c_iter++) { 
                        if (c_iter == 0) {
                                cout << "\n\n\n\n\n\n\n\n\n----------------------\n----------------------\n\n   ������ � :  " << ques_num << '\n';
                                ques_num++;
                                c_iter++;
                        }
                        if (c_iter == query_col_count - 3) { // a column containing first right answer
                            mult_answers = getMultAnswers(); // calling getMultAnswer() method once for both answers
                            this-> right_answers_MC.push_back(*row[c_iter]);
                            if ((mult_answers[0] == right_answers_MC[0]) || (mult_answers[1] == right_answers_MC[0])) {
                                corr_ans_iter++; 
                            }
                            c_iter++;
                        }
                        if (c_iter == query_col_count - 2) { //  a column containing 2nd right answer
                            this-> right_answers_MC.push_back(*row[c_iter]);
                            if ((mult_answers[0] == right_answers_MC[1]) || (mult_answers[1] == right_answers_MC[1])) {
                                corr_ans_iter++; 
                            }
                            c_iter++; 
                        }
                        if (c_iter == query_col_count - 1) {
                            cout << "   ������ ������: ";
                            for(int j=0; j<2; j++) {
                                cout << right_answers_MC[j] << "  ";
                            }
                            right_answers_MC.clear(); // remove all elements from vector before going to next circle of the while-loop
                            cout << "\n\n   ��������� � ������ ������� :\n\n";
                            }
                        cout << row[c_iter]  << '\n';
                        }
                    ans_count = ans_count + 2; //  count of all answers: doubles for MCQ
                    cout << "\n\n\n ����� ������ ������ ������� � ���������� �������, ������� ������.\n";
                    system("pause");
                }
                this-> corr = corr_ans_iter*100/ans_count;  //share of correct answers - TOTAL at thee end of program
                finish = clock(); // timer terminated
            }
            else {
                cout << "mysql_query error";
            }
            mysql_free_result(result);
        }
        else {
        cout << "connection for fetch_data_MC failed";
        }

        return corr; // returns int==percent of correct answers, then it is used by insert_results() to trasfer it to "results table" in the DB

    }


    void insert_results(MYSQL* connection) {
        string test_res; // session result : test passed\not passed
        int min_corr = 70; // minimum percent of correct answers to be obtained for passing the exam

        int sec_el = (this->finish - this->start)/CLOCKS_PER_SEC; // counting seconds elapsed
        int time_limit = 270; // time period that student has at his disposal
        string time_comment;
        if(corr >= min_corr){
            test_res = "��������� ���� ������, ������������ �������� �������";
        }
        else{
            test_res = "��������� ���� �� ������, ������������ �� ��������. ���� ������ ������� ������ ���� �� ����� 70%";
        };
        if(sec_el <= time_limit) {
            time_comment = "�� ��������� � ����������� �� �������";
        }
        else {
            time_comment = "�� ��������� ����������� �� �������";
        }
        string sec_elapsed = to_string(sec_el);
        string corr_share = to_string(corr);

        stringstream ss;
        ss << "INSERT INTO study_sessions(����_������_�������, ����������, ����_�����_���, �����������) VALUES ('"+corr_share+"', '"+test_res+"', '"+sec_elapsed+"', '"+time_comment+"')";
        string res_query = ss.str();
        const char* r_q = res_query.c_str();
        int qstatus = mysql_query(connection, r_q);
        if (!qstatus) {
            cout << "\n\n\n\n   ������������ � ������������ ������ � MySQL Workbench \n\n\n\n";
        }
        else {
            cout << "\n\n   �� ������� �������� ������ � �� \n\n\n";
        }
    }
};

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    MYSQL* connection = connectdatabase();
    Question Q1;

    Q1.fetch_data_WI(connection);
    Q1.fetch_data_SC(connection);
    Q1.fetch_data_MC(connection);

    Q1.insert_results(connection);

    mysql_close(connection); // obligatory calling for function

    return 0;
}


