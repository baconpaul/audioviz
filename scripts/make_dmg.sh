
#export MAC_SIGNING_CERT=
#export MAC_INSTALLING_CERT=

#export MAC_SIGNING_ID=
#export MAC_SIGNING_1UPW=
#export MAC_SIGNING_TEAM=


rm -f /tmp/tmp.dmg
rm -f ${BUILD_DIR}/bin/*.dmg

pushd ${BUILD_DIR}
  mkdir product
  pushd bin
    cp -a audioviz.app ../product
  popd
  pushd product

    pushd audioviz.app/Contents/Frameworks
      pwd
      find . -name "*.framework" -print -exec codesign --force -s "$MAC_SIGNING_CERT" --timestamp -o runtime {} \;
    popd
    codesign --force -s "$MAC_SIGNING_CERT" -o runtime --timestamp --strict "audioviz.app"
    codesign -vvv audioviz.app

  popd
  hdiutil create /tmp/tmp.dmg -ov -volname "AUDIO_VIZ" -fs HFS+ -srcfolder "product"
  hdiutil convert /tmp/tmp.dmg -format UDZO -o "audioviz.dmg"

  codesign --force -s "$MAC_SIGNING_CERT" --timestamp "audioviz.dmg"
  codesign -vvv "audioviz.dmg"

  xcrun notarytool submit "audioviz.dmg" --apple-id ${MAC_SIGNING_ID} --team-id ${MAC_SIGNING_TEAM} --password ${MAC_SIGNING_1UPW} --wait

  xcrun stapler staple "audioviz.dmg"
  mkdir -p dmg
  mv audioviz.dmg dmg

popd
