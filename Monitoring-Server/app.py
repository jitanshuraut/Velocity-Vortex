from flask import Flask, render_template, jsonify
import os
import sqlite3

app = Flask(__name__)

DATA_FOLDER = '/media/sf_QuantWhiz-AlgoEngine/Velocity-Vortex/IO-Server/build/database'


@app.route('/')
def index():
    
    db_files = [f for f in os.listdir(DATA_FOLDER) if f.endswith('.db')]
    return render_template('index.html', db_files=db_files)


@app.route('/data/<filename>')
def get_data(filename):
    file_path = os.path.join(DATA_FOLDER, filename)

    # Check if file exists and has a .db extension
    if os.path.exists(file_path) and filename.endswith('.db'):
        try:
            # Connect to the SQLite database
            conn = sqlite3.connect(file_path)
            cursor = conn.cursor()

            # Query to retrieve table names (assuming you want to display them)
            cursor.execute(
                "SELECT name FROM sqlite_master WHERE type='table';")
            tables = cursor.fetchall()

            # For demonstration, fetching the first 10 rows of the first table
            if tables:
                table_name = tables[0][0]  # Use the first table for example
                cursor.execute(f"SELECT * FROM {table_name} LIMIT 10;")
                rows = cursor.fetchall()

                # Get column names
                column_names = [description[0]
                                for description in cursor.description]
                data = [dict(zip(column_names, row)) for row in rows]

                conn.close()
                return jsonify(data)
            else:
                return jsonify([])  # No tables in the database

        except sqlite3.Error as e:
            return jsonify({"error": str(e)})

    return jsonify([])  # File not found or invalid format


if __name__ == '__main__':
    app.run(debug=True)
