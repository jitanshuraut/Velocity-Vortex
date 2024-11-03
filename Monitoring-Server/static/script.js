document.getElementById("monitorBtn").addEventListener("click", async () => {
  const select = document.getElementById("dbFileSelect");
  const filename = select.value;

  const select_2 = document.getElementById("dbFileSelect_2");
  const filename_2 = select_2.value;

  if (filename) {
    const fetchDataAndUpdate = async () => {
      try {
        const response = await fetch(`/data/${filename}`);
        const data = await response.json();
        document.getElementById("fileContent").innerText = JSON.stringify(
          data,
          null,
          2
        );
      } catch (error) {
        document.getElementById("fileContent").innerText =
          "Error fetching data.";
        console.error("Error:", error);
      }
    };
    await fetchDataAndUpdate();
    setInterval(fetchDataAndUpdate, 5000);
  }

  if (filename_2) {
    const fetchDataAndUpdate = async () => {
      try {
        const response = await fetch(`/data/${filename_2}`);
        const data = await response.json();
        document.getElementById("fileContent_2").innerText = JSON.stringify(
          data,
          null,
          2
        );
      } catch (error) {
        document.getElementById("fileContent_2").innerText =
          "Error fetching data.";
        console.error("Error:", error);
      }
    };
    await fetchDataAndUpdate();
    setInterval(fetchDataAndUpdate, 5000);
  }
});
