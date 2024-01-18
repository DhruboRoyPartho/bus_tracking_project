<?php
    if(isset($_GET['id']) && isset($_GET['lat']) && isset($_GET['long'])){
        $id = $_GET['id'];
        $lat = $_GET['lat'];
        $long = $_GET['long'];

        include("partials/_dbconnect.php");

        $sql = "SELECT COUNT(*) AS SI FROM locations";

        $result = mysqli_query($conn, $sql) or die("error");
        $rows = mysqli_fetch_assoc($result);
        $total_data = $rows['SI'];
        
        if($total_data > 10){
            $sql = "DELETE FROM locations ORDER BY SI LIMIT 1";
            $result = mysqli_query($conn, $sql) or die("dlt error");
        }

        $sql = "INSERT INTO locations (SI, bus_id, latitude, longitude, input_time) VALUES (null, '$id', '$lat', '$long', CURRENT_TIMESTAMP)";
        $result = mysqli_query($conn, $sql);

        if(!$result){
            echo "error";
            die();
        }
        else{
            echo "ok";
            die();
        }
    }
?>

<!-- <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <form action="input.php" method="POST">
        <label for="id">BUS ID</label>
        <input type="text" name="id"><br>
        <label for="lat">Latitude</label>
        <input type="text" name="lat"><br>
        <label for="long">Longitude</label>
        <input type="text" name="long"><br>

        <input type="submit" name="submit" value="Send">
    </form>
</body>
</html> -->