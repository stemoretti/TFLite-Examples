package com.github.stemoretti.tflite_examples;

import org.qtproject.qt.android.bindings.QtActivity;

import android.Manifest;
import android.content.pm.PackageManager;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Iterator;

public class MainActivity extends QtActivity {
    public void checkPermissions() {
        ArrayList<String> permissionsList = new ArrayList<String>();

        permissionsList.add(Manifest.permission.READ_EXTERNAL_STORAGE);

        Iterator it = permissionsList.iterator();
        while (it.hasNext()) {
            int s = ContextCompat.checkSelfPermission(this, (String)it.next());
            if (s == PackageManager.PERMISSION_GRANTED)
                it.remove();
        }

        if (permissionsList.size() > 0) {
            String[] permissions = new String[permissionsList.size()];
            permissionsList.toArray(permissions);
            ActivityCompat.requestPermissions(this, permissions, 101);
        }
    }
}
